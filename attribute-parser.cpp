#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
#include <string>
#include <sstream>
using namespace std;

struct Attribute {
	public:
		string _attr_name;
		string _attr_val;
};

class Tag {
	public:
		virtual ~Tag () {}
		virtual void set_tag_name(string tag_name) {
			_tag_name = tag_name;
		}
		virtual string get_tag_name() {
			return _tag_name;
		}
	private:
		string _tag_name;
};

class OpeningTag: public Tag {
	public:
		void set_attribute (Attribute attr) {
			_attr.push_back(attr);
		}
		vector<Attribute> get_attribute () {
			return _attr;
		}
		void set_child (OpeningTag *child) {
			_child.push_back(*child);
		}
		vector<OpeningTag> get_child () {
			return _child;
		}
	private:
		vector<Attribute> _attr;
		vector<OpeningTag> _child;
};

class ClosingTag: public Tag {
	public:
		void set_opening_tag (OpeningTag *opening_tag) {
			_opening_tag = opening_tag;
		}
		OpeningTag* get_opening_tag () {
			return _opening_tag;
		}
	private:
		OpeningTag *_opening_tag;
};

size_t slice_tag_name (int tag_open_pos, string str, Tag *t, char tag_type) {
	size_t tag_name_end_pos;
	if (tag_type == 'o')
		tag_name_end_pos = str.find(" ", tag_open_pos);
	else
		tag_name_end_pos = str.find(">", tag_open_pos);
	t->set_tag_name(str.substr(tag_open_pos+1, tag_name_end_pos-(tag_open_pos+1)));
	return tag_name_end_pos;
}

size_t slice_attr_name (int tag_name_end_pos, string str, Attribute *a) {
	size_t attr_name_end_pos = str.find("=", tag_name_end_pos);
	a->_attr_name = str.substr(tag_name_end_pos+1, attr_name_end_pos-1-(tag_name_end_pos+1));
	return attr_name_end_pos;
}

size_t slice_attr_val (string str, int attr_name_end_pos, Attribute *a) {
	size_t attr_val_start_pos = str.find("\"", attr_name_end_pos);
	size_t attr_val_end_pos = str.find("\"", attr_val_start_pos+1);
	a->_attr_val = str.substr(attr_val_start_pos+1, attr_val_end_pos-(attr_val_start_pos+1));
	return attr_val_end_pos;
}

int main() {
	// Getting the number of lines of code and the number or lines of queries
	int N, Q;
	cin >> N >> Q;
	cin.ignore();

	// Number of opening tags and closing tags should be half of N
	OpeningTag *ot[N/2];
	ClosingTag *ct[N/2];

	// Create vector to be used as a stack to maintain tag order
	std::vector<OpeningTag*> tag_stack;

	for (int i=0; i<N; i++) {
		// Getting lines of code
		string str;
		getline(cin, str);
		
		char tag_type;
		size_t tag_open_pos = str.find("<", 0);
		if (str.at(tag_open_pos+1) != '/') {
			tag_type = 'o';
			ot[i%(N/2)] = new OpeningTag();
		} else {
			tag_type = 'c';
			tag_open_pos++;
			ct[i%(N/2)] = new ClosingTag();
		}

		// Slicing out tag name
		Tag *t = new Tag();
		if (tag_type == 'o') {
			t = ot[i%(N/2)];
		}
		else if (tag_type == 'c') {
			t = ct[i%(N/2)];
		}
		size_t tag_name_end_pos = slice_tag_name(tag_open_pos, str, t, tag_type);
		if (tag_type == 'o') {
			tag_stack.push_back(ot[i%(N/2)]);
			if (tag_stack.size() != 0) {
				tag_stack.back()->set_child(ot[i%(N/2)]);
			}
		} else if (tag_type == 'c') {
			if (tag_stack.size() != 0 && tag_stack.back()->get_tag_name() == ct[i%(N/2)]->get_tag_name()) {
				tag_stack.pop_back();
			} else {
				cout << "stack empty!" << endl;
			}			
		}

		cout << "top of stack: ";
		cout << tag_stack.back()->get_tag_name() << endl;

		cout << "Stack right now (" << tag_stack.size() << "):" << endl;
		// for (std::vector<OpeningTag*>::iterator it = tag_stack.begin(); it != tag_stack.end(); it++)
		// 	cout << (*it)->get_tag_name() << endl;
		for (int it = 0; it < tag_stack.size(); it++)
			cout << tag_stack.at(it)->get_tag_name() << endl;
		// delete(t);

		// Getting all attributes if the tag is an opening tag
		if (tag_type == 'o') {
			size_t attr_val_end_pos = 0;
			while (str[attr_val_end_pos+1] != '>') {
				// Slicing attribute name
				Attribute a;
				size_t attr_name_end_pos = slice_attr_name(tag_name_end_pos, str, &a);

				// Slicing attribute value
				attr_val_end_pos = slice_attr_val(str, attr_name_end_pos, &a);
				
				// Putting values in the opening tag object
				ot[i%(N/2)]->set_attribute(a);

				// Moving focus to next attribute
				tag_name_end_pos = attr_val_end_pos+1;
			}
		}
	}

	return 0;
}