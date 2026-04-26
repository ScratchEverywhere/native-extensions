import xml.etree.ElementTree as ET
import re

tree = ET.parse("../../../milsko/milsko.xml")
root = tree.getroot()

names = []
for node in root.find("widgets").findall("widget"):
	names.append(node.attrib["name"])

handlers = []
for node in root.find("handlers").findall("handler"):
	handlers.append(node.attrib["name"])

f = open("milsko.js", "r")
data = f.read()
f.close()

autogen = ""
for i in names:
	if len(autogen) > 0:
		autogen = autogen + ", "
	autogen = autogen + "\"" + i + "\""
data = re.sub(r"([ \t]*)\/\* BEGIN WIDGETS \*\/\n[\S\s]*\1\/\* END WIDGETS \*\/\n", r"\1/* BEGIN WIDGETS */\n\1" + autogen + r"\n\1/* END WIDGETS */\n", data)

autogen = ""
for i in handlers:
	if len(autogen) > 0:
		autogen = autogen + ", "
	autogen = autogen + "\"" + i + "\""
data = re.sub(r"([ \t]*)\/\* BEGIN HANDLERS \*\/\n[\S\s]*\1\/\* END HANDLERS \*\/\n", r"\1/* BEGIN HANDLERS */\n\1" + autogen + r"\n\1/* END HANDLERS */\n", data)

for typename in ["integer", "string", "void"]:
	autogen = ""
	for i in root.find("properties"):
		if typename == "void":
			if (i.tag == "integer") or (i.tag == "string"):
				continue
		elif not(i.tag == typename):
			continue
		if len(autogen) > 0:
			autogen = autogen + ", "
		autogen = autogen + "\"" + i.attrib["name"] + "\""
	data = re.sub(r"([ \t]*)\/\* BEGIN " + typename.upper() + r" PROPS \*\/\n[\S\s]*\1\/\* END " + typename.upper() + r" PROPS \*\/\n", r"\1/* BEGIN " + typename.upper() + r" PROPS */\n\1" + autogen + r"\n\1/* END " + typename.upper() + " PROPS */\n", data)

f = open("milsko.js", "w")
f.write(data)
f.close()

f = open("milsko.cpp", "r")
data = f.read()
f.close()

autogen = ""
for i in names:
	autogen = autogen + "	WIDGET(" + i + ");\n"
data = re.sub(r"([ \t]*)\/\* BEGIN WIDGETS \*\/\n[\S\s]*\1\/\* END WIDGETS \*\/\n", r"\1/* BEGIN WIDGETS */\n\1" + autogen + r"\1/* END WIDGETS */\n", data)

autogen = ""
for i in handlers:
	autogen = autogen + "static void " + i + "_handler(MwWidget handle, void* user, void* client){\n"
	autogen = autogen + "	handler(handle, \"" + i + "\", client);\n"
	autogen = autogen + "}\n"
	autogen = autogen + "\n"
data = re.sub(r"([ \t]*)\/\* BEGIN HANDLERS \*\/\n[\S\s]*\1\/\* END HANDLERS \*\/\n", r"\1/* BEGIN HANDLERS */\n\1" + autogen + r"\1/* END HANDLERS */\n", data)

autogen = ""
for i in handlers:
	autogen = autogen + "	MwAddUserHandler(widget, MwN" + i + "Handler, " + i + "_handler, NULL);\n"
data = re.sub(r"([ \t]*)\/\* BEGIN REGISTER HANDLERS \*\/\n[\S\s]*\1\/\* END REGISTER HANDLERS \*\/\n", r"\1/* BEGIN REGISTER HANDLERS */\n\1" + autogen + r"\1/* END REGISTER HANDLERS */\n", data)

f = open("milsko.cpp", "w")
f.write(data)
f.close()
