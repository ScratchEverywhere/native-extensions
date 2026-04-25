#include <os.hpp>
#include <runtime/blocks/blockUtils.hpp>

#define MW_OPENGL_NO_INCLUDE
#define MW_VULKAN_NO_INCLUDE
#include <Mw/Milsko.h>
#include <Mw/Widget/OpenGL.h>
#include <Mw/Widget/Vulkan.h>
#include <stdio.h>

static void* to_ptr(Value src){
	void* dest;

	sscanf(src.asString().c_str(), "%p", (void**)&dest);

	return dest;
}

SCRATCH_BLOCK(milsko, init) {
	Log::log("Milsko initialization");
	MwLibraryInit();
	return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(milsko, step) {
	Value widgetValue;
	MwWidget widget;
	if(!Scratch::getInput(block, "WIDGET", thread, sprite, widgetValue)) return BlockResult::REPEAT;

	widget = (MwWidget)to_ptr(widgetValue);

	*outValue = Value(MwStep(widget) == 0);
	return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(milsko, pending) {
	Value widgetValue;
	MwWidget widget;
	if(!Scratch::getInput(block, "WIDGET", thread, sprite, widgetValue)) return BlockResult::REPEAT;

	widget = (MwWidget)to_ptr(widgetValue);

	*outValue = Value(!!MwPending(widget));
	return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(milsko, none) {
	char cptr[16];

	sprintf(cptr, "%p", NULL);

	*outValue = Value(std::string(cptr));
	return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(milsko, default) {
	*outValue = Value(MwDEFAULT);
	return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(milsko, move) {
	Value widgetValue, xValue, yValue;
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
	Value widgetValue, widthValue, heightValue;
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

SCRATCH_SHADOW_BLOCK(milsko_menu_WIDGET_CLASS, WIDGET_CLASS);

#define TYPE(type, mtype, stype, prefix, stuff) \
SCRATCH_SHADOW_BLOCK(milsko_menu_ ## stype ## _PROP, stype ## _PROP); \
SCRATCH_BLOCK(milsko, set ## type) { \
	Value widgetValue, propValue, valueValue; \
	MwWidget widget; \
	if(!Scratch::getInput(block, "WIDGET", thread, sprite, widgetValue)) return BlockResult::REPEAT; \
	if(!Scratch::getInput(block, "PROP", thread, sprite, propValue)) return BlockResult::REPEAT; \
	if(!Scratch::getInput(block, "VALUE", thread, sprite, valueValue)) return BlockResult::REPEAT; \
\
	widget = (MwWidget)to_ptr(widgetValue); \
	MwSet ## mtype(widget, (prefix + propValue.asString()).c_str(), stuff); \
\
	return BlockResult::CONTINUE; \
}

TYPE(Integer, Integer, INTEGER, "I", (int)valueValue.asDouble());
TYPE(String, Text, STRING, "S", valueValue.asString().c_str());
TYPE(Void, Void, VOID, "V", to_ptr(valueValue));

SCRATCH_BLOCK(milsko, create) {
	Value widgetClassValue, nameValue, xValue, yValue, widthValue, heightValue, parentValue;
	MwWidget widget, parent;
	char cptr[64];
	MwClass widget_class = NULL;
	if(!Scratch::getInput(block, "WIDGET_CLASS", thread, sprite, widgetClassValue)) return BlockResult::REPEAT;
	if(!Scratch::getInput(block, "NAME", thread, sprite, nameValue)) return BlockResult::REPEAT;
	if(!Scratch::getInput(block, "X", thread, sprite, xValue)) return BlockResult::REPEAT;
	if(!Scratch::getInput(block, "Y", thread, sprite, yValue)) return BlockResult::REPEAT;
	if(!Scratch::getInput(block, "WIDTH", thread, sprite, widthValue)) return BlockResult::REPEAT;
	if(!Scratch::getInput(block, "HEIGHT", thread, sprite, heightValue)) return BlockResult::REPEAT;
	if(!Scratch::getInput(block, "PARENT", thread, sprite, parentValue)) return BlockResult::REPEAT;

	std::cout << widgetClassValue.asString() << std::endl;

#define WIDGET(x) \
	if(widgetClassValue.asString() == #x){ \
		widget_class = Mw ## x ## Class; \
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
	
	sprintf(cptr, "%p", widget);
	
	*outValue = Value(std::string(cptr));
	return BlockResult::CONTINUE;
}
