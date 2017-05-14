import sys
from xml.dom import minidom

argv = sys.argv
svg_file = argv[1]

svg_output = open("adoblified.svg", "w")

doc = minidom.parse(svg_file)  # parseString also exists

shape_ids = []
path_strings = []

for path in doc.getElementsByTagName('polygon'):
	shape_id = path.getAttribute('id').split('_')[0]
	if shape_id not in shape_ids:
		shape_ids.append(shape_id)
		svg_output.write(path.toprettyxml()+"\n")

doc.unlink()
svg_output.close()