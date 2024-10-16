import gdb

class GlmVec4Printer:

	def __init__(self, val):
		self.val = val

	def to_string(self):
		x = self.val['x']
		y = self.val['y']
		z = self.val['z']
		w = self.val['w']
		return f"glm::vec4({x}, {y}, {z}, {w})"

def register_glm_printers(objfile):
	objfile.pretty_printers.append(lookup_type)

def lookup_type(val):
	basic_type = gdb.types.get_basic_type(val.type)
	if basic_type.tag == 'glm::vec4':
		return GlmVec4Printer(val)
	return None

glm.pretty_printers.append(lookup_type)
