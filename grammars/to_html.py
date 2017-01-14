import json
from pprint import pprint

with open('test_time_grammar_test_port.json') as data_file:    
	data = json.load(data_file)
	for r_idx, rule in enumerate(data['rules']):
		if rule["rule_name"].startswith("time"):
			s_params = ""
			for p in rule["parameters"][:-1]:
				s_params += str(p) + ", "
			s_params += str(rule["parameters"][-1])

			shape_tags = rule["init_value"] 
			if shape_tags=="":
				shape_tags="all"

			ts_tags = ""
			for p in rule["matching_tags"][:-1]:
				ts_tags += str(p) + ", "
			ts_tags += str(rule["matching_tags"][-1])
			if ts_tags=="":
				ts_tags="all"
			
			prod_tags = ""	
			for p in rule["produced_tags"][:-1]:
				prod_tags += str(p) + ", "
			prod_tags += str(rule["produced_tags"][-1])

			print("<li><span id=\"rule\">")
			print("\ttimeslice({"+s_params+"}) : [('"+ts_tags+"'), ('"+shape_tags+"')] -> ['"+prod_tags+"']")
			print("</span></li>")	
		elif rule["rule_name"].startswith("space"):
			ts_tags = rule["init_value"] 
			if ts_tags=="":
				ts_tags="all"

			shape_tags = ""
			for p in rule["matching_tags"][:-1]:
				shape_tags += str(p) + ", "
			shape_tags += str(rule["matching_tags"][-1])
			if shape_tags=="":
				shape_tags="all"
			
			sin_glo = "group" if rule["parameters"][0] == 0.0 else "single"

			print("<li><span id=\"rule\">")
			print("\tanimate('affine', "+sin_glo+", "+str(rule["parameters"][-1])+" , "+rule["operator"]+"("+str(rule["parameters"][1:-1])+")) : [('"+ts_tags+"'), ('"+shape_tags+"')] -> ['term_"+str(r_idx)+"']")
			print("</span></li>")	
