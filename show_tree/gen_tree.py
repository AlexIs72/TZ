#!/usr/bin/python


import json
import sys
import random
import time
import struct
# https://anytree.readthedocs.io/en/2.4.3/installation.html
from anytree import Node, RenderTree
from anytree.exporter import JsonExporter


tree_depth = 5
min_child_nodes = 2
max_child_nodes = 5
output_format = 0   # 0 - text
                    # 1 - binary

def get_random_node(level):
    # 1 - string; 2 - int; 3 - float
    node =  {'type': random.randint(1, 3), 'value': None, 'children': []} #Node("node_level_" + str(level))
#    node.value_type = random.randint(1, 3)
   
#    if parent != None:
#        node.parent = parent

    if node['type'] == 1:
        node['value'] = "string_" + str(random.randint(0, 1000))
    elif node['type'] == 2:
        node['value'] = random.randint(1, 100)
    elif node['type'] == 3: 
        node['value'] = random.uniform(1.0, 100.0) 

    return node

#    if node_type == 0:
#        return Node("node_level_" + level, node_type, )


def gen_node(level, parent_node):

    if(level >= tree_depth):
        return
    
    num_child_nodes = random.randint(min_child_nodes, max_child_nodes)


    for index in range(num_child_nodes):
        node = get_random_node(level)
        gen_node(level+1, node)
        if len(node['children']) == 0:
			del node['children']
        parent_node['children'].append(node)

def usage(prog_name):
    print("Usage: " + prog_name + " <help|text|binary> <file_name>")
    print("   text output by default")
	

def dump_node_to_binary(f, node):
	f.write(struct.pack('<b', node['type']))
	# 1 - string; 2 - int; 3 - float
	if node['type'] == 1:
		f.write(struct.pack('<i', len(node['value'])))
		f.write(node['value'])
	elif node['type'] == 2:
		f.write(struct.pack('<i', node['value']))
	elif node['type'] == 3:
		f.write(struct.pack('<f', node['value']))

	
	if 'children' in node:
		f.write(struct.pack('<i', len(node['children'])))
		for index in range(len(node['children'])):
			dump_node_to_binary(f, node['children'][index])
	else:
		f.write(struct.pack('<i', 0))


#-----------------------------------------------------------------

if len(sys.argv) > 1:
    if sys.argv[1] == "help":
        usage(sys.argv[0])
        sys.exit(0)
    elif sys.argv[1] == "text": 
        output_format = 0
    elif sys.argv[1] == "binary":    
        output_format = 1
    else:
        usage(sys.argv[0])
        sys.exit(0)

if sys.argv[2:]:
	file_name = sys.argv[2]
else:
	file_name = "tree_data"
	if output_format == 0:
		file_name = file_name + ".json"
	else:
		file_name = file_name + ".bin"


random.seed();

root = get_random_node(0)

gen_node(1, root)

if output_format == 0:
#    exporter = JsonExporter(indent=2)
#    print(exporter.export(root))
#	root = json.JSONEncoder(root);
#	print(root)
	with open(file_name, 'w') as f:
		json.dump(root, f, indent=2)
		f.close()
else:
	print(json.dumps(root, indent=2))
	with open(file_name, 'wb') as f:
		dump_node_to_binary(f, root)
		f.close()		


