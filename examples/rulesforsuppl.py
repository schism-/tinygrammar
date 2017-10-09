import json
from pprint import pprint
import sys

argv = sys.argv
grammar = argv[1]

with open(grammar) as data_file:    
    data = json.load(data_file)

for rule in data["rules"]:
	if(rule["operator"] == "time_slice"):
		operator = "timesplit({";
		parameters = ','.join(str(e) for e in rule["parameters"])
		parameters+="}) : [("+rule["matching_tags"][0]+"), ("
		matching_tags = rule["init_value"] if rule["init_value"] != "" else "all"
		matching_tags+= ")] -> ["
		produced_tags = ",".join(str(e) for e in rule["produced_tags"])
		produced_tags+="]"
		print("<li><span id=\"rule\">\n"+operator+parameters+matching_tags+produced_tags+"\n</span></li>\n")
	elif(rule["operator"] == "attribute"):
		operator = "animate(affine, "
		operator += "single, " if rule["parameters"][len(rule["parameters"])-2] == 1.0 else "group, "
		operator += str(rule["parameters"][len(rule["parameters"])-1])+" , "
		operator += rule["operator"]+"(["
		parameters = ','.join(str(e) for e in rule["parameters"][:-2])
		parameters+="])) : [("+rule["init_value"]+"), ("
		matching_tags = ','.join(str(e) for e in rule["matching_tags"])
		matching_tags+= ")] -> ["
		produced_tags = ",".join(str(e) for e in rule["produced_tags"])
		produced_tags+="]"
		print("<li><span id=\"rule\">\n"+operator+parameters+matching_tags+produced_tags+"\n</span></li>\n")
	else:
		operator = "animate(affine, "
		operator += "single, " if rule["parameters"][0] == 1.0 else "group, "
		operator += str(rule["parameters"][len(rule["parameters"])-1])+" , "
		operator += rule["operator"]+"(["
		parameters = ','.join(str(e) for e in rule["parameters"][1:-1])
		parameters+="])) : [("+rule["init_value"]+"), ("
		matching_tags = ','.join(str(e) for e in rule["matching_tags"])
		matching_tags+= ")] -> ["
		produced_tags = ",".join(str(e) for e in rule["produced_tags"])
		produced_tags+="]"
		print("<li><span id=\"rule\">\n"+operator+parameters+matching_tags+produced_tags+"\n</span></li>\n")


#animate(affine, group, 0.0 , translation([-20.0, 0.0])) : [(a6)] -> [cs5_e1]
	
