#include "ConFinat.hpp"

void printObject(ConFinat::ConfData data) {
	for (auto section : data) {
		if (section.values.empty()) continue;
		printf("New Section: %s\n", section.name.c_str());
		for (auto pair : section.values) {
			printf("%s = ", pair.key.c_str());
			if (pair.value.type == ConFinat::__STRING) {
				printf("\"%s\"\n", pair.value.string.c_str());
				continue;
			}
			if (pair.value.type == ConFinat::__NUMBER) {
				printf("%f\n", pair.value.number);
				continue;
			}
			if (pair.value.type == ConFinat::__BOOLEAN) {
				if (pair.value.boolean) printf("true\n");
				else printf("false\n");
				continue;
			}
			printf("{");
			for (auto i : pair.value.vector) {
				printf("%.0f, ", i);
			}
			printf("}\n");
		}
	}
}

int main() {

	ConFinat::ConfData data;
	data = ConFinat::parseFromFile("example.cft");
	printObject(data);

	std::string multilineString = R"(
file = False
[Section1]
text = "This text object comes from a string"
# long vector
numbers = (23, 10);
; And an empty line has no issue

number = -123456789e-6)";

	data = ConFinat::parseFromString(multilineString, "text");
	printObject(data);


}
