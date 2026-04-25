(function (Scratch) {
 	let widgets = [
		/* BEGIN WIDGETS */
		"Box", "Button", "CheckBox", "Entry", "Frame", "Image", "Label", "ListBox", "Menu", "NumberEntry", "ScrollBar", "SubMenu", "Viewport", "Window", "ProgressBar", "RadioBox", "ComboBox", "TreeView", "OpenGL", "Vulkan"
		/* END WIDGETS */
	];

	class Milsko {
		getInfo() {
			return {
				id: "milsko",
				name: "Milsko",
				menus: {
					INTEGER_PROP: {
						acceptReporters: true,
						items: [
							/* BEGIN INTEGER PROPS */
							"x", "y", "width", "height", "orientation", "minValue", "maxValue", "value", "changedBy", "areaShown", "checked", "alignment", "bold", "main", "leftPadding", "hasHeading", "hasBorder", "inverted", "modernLook", "hideInput", "singleClickSelectable", "flat", "showArrows", "padding", "borderWidth", "ratio", "fixedSize", "margin", "bitmapFont", "sevenSegment", "length", "forceInverted", "isRounded", "darkTheme", "useMonospace", "fillArea", "waitMS"
							/* END INTEGER PROPS */
						]
					},
					STRING_PROP: {
						acceptReporters: true,
						items: [
							/* BEGIN STRING PROPS */
							"title", "text", "background", "subBackground", "foreground", "subForeground", "vulkanExtension", "vulkanLayer"
							/* END STRING PROPS */
						]
					},
					VOID_PROP: {
						acceptReporters: true,
						items: [
							/* BEGIN VOID PROPS */
							"pixmap", "iconPixmap", "sizeHints", "font", "boldFont", "backgroundPixmap", "monospaceFont", "boldMonospaceFont", "vulkanConfig"
							/* END VOID PROPS */
						]
					},
					WIDGET_CLASS: {
						acceptReporters: false,
						items: widgets
					}
				},
				blocks: [
					{
						opcode: "init",
						blockType: Scratch.BlockType.COMMAND,
						text: "Initialize Milsko"
					},
					{
						opcode: "step",
						blockType: Scratch.BlockType.BOOLEAN,
						text: "Ran a single step for [WIDGET] and widgets want to keep running?",
						arguments: {
							WIDGET: {
								type: Scratch.ArgumentType.STRING
							}
						}
					},
					{
						opcode: "pending",
						blockType: Scratch.BlockType.BOOLEAN,
						text: "Any pending events for [WIDGET]?",
						arguments: {
							WIDGET: {
								type: Scratch.ArgumentType.STRING
							}
						}
					},
					{
						opcode: "none",
						blockType: Scratch.BlockType.REPORTER,
						text: "None"
					},
					{
						opcode: "default",
						blockType: Scratch.BlockType.REPORTER,
						text: "Default"
					},
					{
						opcode: "move",
						blockType: Scratch.BlockType.COMMAND,
						text: "Move widget [WIDGET] to ([X], [Y])",
						arguments: {
							WIDGET: {
								type: Scratch.ArgumentType.STRING
							},
							X: {
								type: Scratch.ArgumentType.NUMBER
							},
							Y: {
								type: Scratch.ArgumentType.NUMBER
							}
						}
					},
					{
						opcode: "resize",
						blockType: Scratch.BlockType.COMMAND,
						text: "Resize widget [WIDGET] to [WIDTH]x[HEIGHT]",
						arguments: {
							WIDGET: {
								type: Scratch.ArgumentType.STRING
							},
							WIDTH: {
								type: Scratch.ArgumentType.NUMBER
							},
							HEIGHT: {
								type: Scratch.ArgumentType.NUMBER
							}
						}
					},
					{
						opcode: "setInteger",
						blockType: Scratch.BlockType.COMMAND,
						text: "Set integer property [PROP] of widget [WIDGET] to [VALUE]",
						arguments: {
							WIDGET: {
								type: Scratch.ArgumentType.STRING
							},
							PROP: {
								type: Scratch.ArgumentType.STRING,
								menu: "INTEGER_PROP"
							},
							VALUE: {
								type: Scratch.ArgumentType.NUMBER
							}
						}
					},
					{
						opcode: "setString",
						blockType: Scratch.BlockType.COMMAND,
						text: "Set string property [PROP] of widget [WIDGET] to [VALUE]",
						arguments: {
							WIDGET: {
								type: Scratch.ArgumentType.STRING
							},
							PROP: {
								type: Scratch.ArgumentType.STRING,
								menu: "STRING_PROP"
							},
							VALUE: {
								type: Scratch.ArgumentType.STRING
							}
						}
					},
					{
						opcode: "setVoid",
						blockType: Scratch.BlockType.COMMAND,
						text: "Set void property [PROP] of widget [WIDGET] to [VALUE]",
						arguments: {
							WIDGET: {
								type: Scratch.ArgumentType.STRING
							},
							PROP: {
								type: Scratch.ArgumentType.STRING,
								menu: "VOID_PROP"
							},
							VALUE: {
								type: Scratch.ArgumentType.STRING
							}
						}
					},
					{
						opcode: "create",
						blockType: Scratch.BlockType.REPORTER,
						text: "Creates a [WIDGET_CLASS] widget named [NAME] at ([X], [Y]) with size [WIDTH]x[HEIGHT] as the child of [PARENT]",
						arguments: {
							WIDGET_CLASS: {
								type: Scratch.ArgumentType.STRING,
								menu: "WIDGET_CLASS",
							},
							NAME: {
								type: Scratch.ArgumentType.STRING
							},
							X: {
								type: Scratch.ArgumentType.NUMBER
							},
							Y: {
								type: Scratch.ArgumentType.NUMBER
							},
							WIDTH: {
								type: Scratch.ArgumentType.NUMBER
							},
							HEIGHT: {
								type: Scratch.ArgumentType.NUMBER
							},
							PARENT: {
								type: Scratch.ArgumentType.STRING
							}
						}
					}
				]
			};
		}

		init() {
		}

		step() {
		}

		pending() {
		}

		setInteger() {
		}

		setText() {
		}

		setVoid() {
		}

		create() {
		}
	};

	Scratch.extensions.register(new Milsko());
})(Scratch);
