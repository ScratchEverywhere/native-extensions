#include <os.hpp>
#include <runtime/blocks/blockUtils.hpp>

#define MW_OPENGL_NO_INCLUDE
#define MW_VULKAN_NO_INCLUDE
#include <Mw/Milsko.h>
#include <Mw/Widget/OpenGL.h>
#include <Mw/Widget/Vulkan.h>
#include <stdio.h>

struct widget_context {
	std::vector<ScriptThread*>	   threads;
	std::map<std::string, std::string> mapping;

	Sprite* sprite;
};

static void* to_ptr(Value src) {
	void* dest;
	sscanf(src.asString().c_str(), "%p", (void**)&dest);

	return dest;
}

static std::string from_ptr(void* ptr) {
	char cptr[64];

	sprintf(cptr, "%p", ptr);

	return std::string(cptr);
}

SCRATCH_BLOCK(milsko, init) {
	Log::log("Milsko initialization");
	MwLibraryInit();
	return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(milsko, step) {
	Value	 widgetValue;
	MwWidget widget;
	if(!Scratch::getInput(block, "WIDGET", thread, sprite, widgetValue)) return BlockResult::REPEAT;

	widget			= (MwWidget)to_ptr(widgetValue);
	widget_context* context = (widget_context*)MwGetUser(widget);

	if(context->threads.size() == 0) *outValue = Value(MwStep(widget) == 0);

	for(int i = 0; i < context->threads.size(); i++) {
		BlockResult result = BlockExecutor::runThread(*context->threads[i], *sprite, nullptr);

		if(result == BlockResult::RETURN || context->threads[i]->finished) {
			context->threads[i]->clear();

			delete context->threads[i];
			context->threads.erase(context->threads.begin() + i);

			i--;
		}
	}

	if(context->threads.size() == 0) {
		thread->eraseState(block);
	} else {
		return BlockResult::REPEAT;
	}

	return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(milsko, pending) {
	Value	 widgetValue;
	MwWidget widget;
	if(!Scratch::getInput(block, "WIDGET", thread, sprite, widgetValue)) return BlockResult::REPEAT;

	widget = (MwWidget)to_ptr(widgetValue);

	*outValue = Value(!!MwPending(widget));
	return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(milsko, none) {
	*outValue = Value(from_ptr(NULL));
	return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(milsko, default) {
	*outValue = Value(MwDEFAULT);
	return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(milsko, destroy) {
	Value	 widgetValue;
	MwWidget widget;
	if(!Scratch::getInput(block, "WIDGET", thread, sprite, widgetValue)) return BlockResult::REPEAT;

	widget = (MwWidget)to_ptr(widgetValue);
	if(MwGetParent(widget) == NULL) {
		delete(widget_context*)MwGetUser(widget);
	}
	MwDestroyWidget(widget);

	return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(milsko, move) {
	Value	 widgetValue, xValue, yValue;
	MwWidget widget;
	if(!Scratch::getInput(block, "WIDGET", thread, sprite, widgetValue)) return BlockResult::REPEAT;
	if(!Scratch::getInput(block, "X", thread, sprite, xValue)) return BlockResult::REPEAT;
	if(!Scratch::getInput(block, "Y", thread, sprite, yValue)) return BlockResult::REPEAT;

	widget = (MwWidget)to_ptr(widgetValue);
	MwVaApply(widget,
		  MwNx, (int)xValue.asDouble(),
		  MwNy, (int)yValue.asDouble(),
		  NULL);

	return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(milsko, resize) {
	Value	 widgetValue, widthValue, heightValue;
	MwWidget widget;
	if(!Scratch::getInput(block, "WIDGET", thread, sprite, widgetValue)) return BlockResult::REPEAT;
	if(!Scratch::getInput(block, "WIDTH", thread, sprite, widthValue)) return BlockResult::REPEAT;
	if(!Scratch::getInput(block, "HEIGHT", thread, sprite, heightValue)) return BlockResult::REPEAT;

	widget = (MwWidget)to_ptr(widgetValue);
	MwVaApply(widget,
		  MwNwidth, (int)widthValue.asDouble(),
		  MwNheight, (int)heightValue.asDouble(),
		  NULL);

	return BlockResult::CONTINUE;
}

SCRATCH_SHADOW_BLOCK(milsko_menu_EVENT_TYPE, EVENT_TYPE);

SCRATCH_BLOCK(milsko, register) {
	Value	 customValue, eventValue, widgetValue;
	MwWidget widget;
	if(!Scratch::getInput(block, "CUSTOM", thread, sprite, customValue)) return BlockResult::REPEAT;
	if(!Scratch::getInput(block, "EVENT", thread, sprite, eventValue)) return BlockResult::REPEAT;
	if(!Scratch::getInput(block, "WIDGET", thread, sprite, widgetValue)) return BlockResult::REPEAT;

	widget = (MwWidget)to_ptr(widgetValue);

	((widget_context*)MwGetUser(widget))->mapping[eventValue.asString()] = customValue.asString();

	return BlockResult::CONTINUE;
}

SCRATCH_SHADOW_BLOCK(milsko_menu_WIDGET_CLASS, WIDGET_CLASS);

#define TYPE(type, mtype, stype, prefix, stuff, stuff2) \
	SCRATCH_SHADOW_BLOCK(milsko_menu_##stype##_PROP, stype##_PROP); \
	SCRATCH_BLOCK(milsko, set##type) { \
		Value	 widgetValue, propValue, valueValue; \
		MwWidget widget; \
		if(!Scratch::getInput(block, "WIDGET", thread, sprite, widgetValue)) return BlockResult::REPEAT; \
		if(!Scratch::getInput(block, "PROP", thread, sprite, propValue)) return BlockResult::REPEAT; \
		if(!Scratch::getInput(block, "VALUE", thread, sprite, valueValue)) return BlockResult::REPEAT; \
\
		widget = (MwWidget)to_ptr(widgetValue); \
		MwSet##mtype(widget, (prefix + propValue.asString()).c_str(), stuff); \
\
		return BlockResult::CONTINUE; \
	} \
	SCRATCH_BLOCK(milsko, get##type) { \
		Value	 widgetValue, propValue; \
		MwWidget widget; \
		if(!Scratch::getInput(block, "WIDGET", thread, sprite, widgetValue)) return BlockResult::REPEAT; \
		if(!Scratch::getInput(block, "PROP", thread, sprite, propValue)) return BlockResult::REPEAT; \
\
		widget = (MwWidget)to_ptr(widgetValue); \
\
		*outValue = Value(stuff2(MwGet##mtype(widget, (prefix + propValue.asString()).c_str()))); \
		return BlockResult::CONTINUE; \
	}

TYPE(Integer, Integer, INTEGER, "I", (int)valueValue.asDouble(), (int));
TYPE(String, Text, STRING, "S", valueValue.asString().c_str(), std::string);
TYPE(Void, Void, VOID, "V", to_ptr(valueValue), from_ptr);

static void handler(MwWidget handle, std::string name, void* client) {
	widget_context* context = (widget_context*)MwGetUser(handle);
	ScriptThread*	newThread;

	if(context->mapping.find(name) == context->mapping.end()) return;
	if(context->sprite->customHatBlock.find(name) == context->sprite->customHatBlock.end()) return;

	if(!Pools::threads.empty()) {
		newThread = Pools::threads.back();
		Pools::threads.pop_back();
	} else {
		newThread = new ScriptThread();
	}
	newThread->blockHat		= context->sprite->customHatBlock[context->mapping[name]];
	newThread->nextBlock		= newThread->blockHat;
	newThread->withoutScreenRefresh = true;
	newThread->finished		= false;
	newThread->returnValue		= Value();
	newThread->MyBlocksVariablen.clear();

	/* TODO: do argument stuff... */

	context->threads.push_back(newThread);
}

/* BEGIN HANDLERS */
static void activate_handler(MwWidget handle, void* user, void* client) {
	handler(handle, "activate", client);
}

static void listBoxActivate_handler(MwWidget handle, void* user, void* client) {
	handler(handle, "listBoxActivate", client);
}

static void treeViewActivate_handler(MwWidget handle, void* user, void* client) {
	handler(handle, "treeViewActivate", client);
}

static void resize_handler(MwWidget handle, void* user, void* client) {
	handler(handle, "resize", client);
}

static void tick_handler(MwWidget handle, void* user, void* client) {
	handler(handle, "tick", client);
}

static void menu_handler(MwWidget handle, void* user, void* client) {
	handler(handle, "menu", client);
}

static void mouseDown_handler(MwWidget handle, void* user, void* client) {
	handler(handle, "mouseDown", client);
}

static void mouseUp_handler(MwWidget handle, void* user, void* client) {
	handler(handle, "mouseUp", client);
}

static void mouseMove_handler(MwWidget handle, void* user, void* client) {
	handler(handle, "mouseMove", client);
}

static void changed_handler(MwWidget handle, void* user, void* client) {
	handler(handle, "changed", client);
}

static void comboBoxChanged_handler(MwWidget handle, void* user, void* client) {
	handler(handle, "comboBoxChanged", client);
}

static void key_handler(MwWidget handle, void* user, void* client) {
	handler(handle, "key", client);
}

static void keyRelease_handler(MwWidget handle, void* user, void* client) {
	handler(handle, "keyRelease", client);
}

static void close_handler(MwWidget handle, void* user, void* client) {
	handler(handle, "close", client);
}

static void focusIn_handler(MwWidget handle, void* user, void* client) {
	handler(handle, "focusIn", client);
}

static void focusOut_handler(MwWidget handle, void* user, void* client) {
	handler(handle, "focusOut", client);
}

static void fileChosen_handler(MwWidget handle, void* user, void* client) {
	handler(handle, "fileChosen", client);
}

static void directoryChosen_handler(MwWidget handle, void* user, void* client) {
	handler(handle, "directoryChosen", client);
}

static void colorChosen_handler(MwWidget handle, void* user, void* client) {
	handler(handle, "colorChosen", client);
}

static void draw_handler(MwWidget handle, void* user, void* client) {
	handler(handle, "draw", client);
}

static void clipboard_handler(MwWidget handle, void* user, void* client) {
	handler(handle, "clipboard", client);
}

static void darkTheme_handler(MwWidget handle, void* user, void* client) {
	handler(handle, "darkTheme", client);
}

/* END HANDLERS */

SCRATCH_BLOCK(milsko, create) {
	Value	 widgetClassValue, nameValue, xValue, yValue, widthValue, heightValue, parentValue;
	MwWidget widget, parent, topmost;
	MwClass	 widget_class = NULL;
	if(!Scratch::getInput(block, "WIDGET_CLASS", thread, sprite, widgetClassValue)) return BlockResult::REPEAT;
	if(!Scratch::getInput(block, "NAME", thread, sprite, nameValue)) return BlockResult::REPEAT;
	if(!Scratch::getInput(block, "X", thread, sprite, xValue)) return BlockResult::REPEAT;
	if(!Scratch::getInput(block, "Y", thread, sprite, yValue)) return BlockResult::REPEAT;
	if(!Scratch::getInput(block, "WIDTH", thread, sprite, widthValue)) return BlockResult::REPEAT;
	if(!Scratch::getInput(block, "HEIGHT", thread, sprite, heightValue)) return BlockResult::REPEAT;
	if(!Scratch::getInput(block, "PARENT", thread, sprite, parentValue)) return BlockResult::REPEAT;

#define WIDGET(x) \
	if(widgetClassValue.asString() == #x) { \
		widget_class = Mw##x##Class; \
	}

	/* BEGIN WIDGETS */
	WIDGET(Box);
	WIDGET(Button);
	WIDGET(CheckBox);
	WIDGET(Entry);
	WIDGET(Frame);
	WIDGET(Image);
	WIDGET(Label);
	WIDGET(ListBox);
	WIDGET(Menu);
	WIDGET(NumberEntry);
	WIDGET(ScrollBar);
	WIDGET(SubMenu);
	WIDGET(Viewport);
	WIDGET(Window);
	WIDGET(ProgressBar);
	WIDGET(RadioBox);
	WIDGET(ComboBox);
	WIDGET(TreeView);
	WIDGET(OpenGL);
	WIDGET(Vulkan);
	/* END WIDGETS */

#undef WIDGET

	parent = to_ptr(parentValue);

	widget = MwCreateWidget(widget_class, nameValue.asString().c_str(), parent, (int)(xValue.asDouble()), (int)(yValue.asDouble()), (unsigned int)(widthValue.asDouble()), (unsigned int)(heightValue.asDouble()));

	/* BEGIN REGISTER HANDLERS */
	MwAddUserHandler(widget, MwNactivateHandler, activate_handler, NULL);
	MwAddUserHandler(widget, MwNlistBoxActivateHandler, listBoxActivate_handler, NULL);
	MwAddUserHandler(widget, MwNtreeViewActivateHandler, treeViewActivate_handler, NULL);
	MwAddUserHandler(widget, MwNresizeHandler, resize_handler, NULL);
	MwAddUserHandler(widget, MwNtickHandler, tick_handler, NULL);
	MwAddUserHandler(widget, MwNmenuHandler, menu_handler, NULL);
	MwAddUserHandler(widget, MwNmouseDownHandler, mouseDown_handler, NULL);
	MwAddUserHandler(widget, MwNmouseUpHandler, mouseUp_handler, NULL);
	MwAddUserHandler(widget, MwNmouseMoveHandler, mouseMove_handler, NULL);
	MwAddUserHandler(widget, MwNchangedHandler, changed_handler, NULL);
	MwAddUserHandler(widget, MwNcomboBoxChangedHandler, comboBoxChanged_handler, NULL);
	MwAddUserHandler(widget, MwNkeyHandler, key_handler, NULL);
	MwAddUserHandler(widget, MwNkeyReleaseHandler, keyRelease_handler, NULL);
	MwAddUserHandler(widget, MwNcloseHandler, close_handler, NULL);
	MwAddUserHandler(widget, MwNfocusInHandler, focusIn_handler, NULL);
	MwAddUserHandler(widget, MwNfocusOutHandler, focusOut_handler, NULL);
	MwAddUserHandler(widget, MwNfileChosenHandler, fileChosen_handler, NULL);
	MwAddUserHandler(widget, MwNdirectoryChosenHandler, directoryChosen_handler, NULL);
	MwAddUserHandler(widget, MwNcolorChosenHandler, colorChosen_handler, NULL);
	MwAddUserHandler(widget, MwNdrawHandler, draw_handler, NULL);
	MwAddUserHandler(widget, MwNclipboardHandler, clipboard_handler, NULL);
	MwAddUserHandler(widget, MwNdarkThemeHandler, darkTheme_handler, NULL);
	/* END REGISTER HANDLERS */

	topmost = widget;
	while(MwGetParent(topmost) != NULL) {
		topmost = MwGetParent(topmost);
	}

	if(topmost == widget) {
		widget_context* context = new widget_context();
		context->sprite		= sprite;
		MwSetUser(widget, context);
	} else {
		MwSetUser(widget, MwGetUser(topmost));
	}

	*outValue = Value(from_ptr((void*)widget));
	return BlockResult::CONTINUE;
}
