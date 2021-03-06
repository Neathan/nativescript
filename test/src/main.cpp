
#include <ns/loader.h>


int main() {
	using namespace ns;

	auto collection = ScriptCollection::create("testlib.dll");
	if (collection) {
		for (const auto& [name, script] : collection->getScripts()) {
			printf("Script name: %s\n", name.c_str());

			Script* s = script.createScriptFunc();
			s->start();
			for (int i = 0; i < 10; ++i) {
				if (i == 5) {
					script.parameters[0].set<int>(s, 0);
				}
				s->update();

				int x = script.parameters[0].get<int>(s);
				printf("Retrieved value: %d\n", x);
			}
			script.destroyScriptFunc(s);
		}
	}
	
	return 0;
}