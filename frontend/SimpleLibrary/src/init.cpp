#include <core/Godot.hpp>
#include <Reference.hpp>

#include <vector>

using namespace godot;

class SimpleClass : public GodotScript<Reference> {
  GODOT_CLASS(SimpleClass);
public:
  SimpleClass() { }

  void test_void_method() {
    Godot::print("This is test");
  }

  Variant method(Variant arg) {
    Variant ret;
    ret = arg;

    return ret;
  }

  void receive_matrix (Array matrix) {
    Godot::print("Received");
    for (int i = 0; i < matrix.size(); i++) {
      Array row = matrix[i];
      for (int j = 0; j < row.size(); j++) {
        Godot::print(row[j]);
      }
    }
  }

  Array send_matrix (void) {
    Array matrix;
    for (int i = 0; i < 3; i++) {
      Array aux;
      for (int j = 0; j < 3; j++) {
        aux.push_back ((i * 3) + j);
      }
      matrix.push_back(aux);
    }
    Godot::print("Send");
    return matrix;
  }

  void set_punctuations (Array matrix) {

  }

  Array get_new_poblation (void) {

  }

  static void _register_methods() {
    register_method("method", &SimpleClass::method);
    register_method("test",  &SimpleClass::test_void_method);
    register_method("set_puntuations",  &SimpleClass::receive_matrix);
    register_method("get_new_poblation",  &SimpleClass::send_matrix);

    /**
     * How to register exports like gdscript
     * export var _name = "SimpleClass"
     **/
    register_property((char *)"base/name", &SimpleClass::_name, String("SimpleClass"));

    /** For registering signal **/
    // register_signal<SimpleClass>("signal_name");
  }

  String _name;
};

/** GDNative Initialize **/
GDNATIVE_INIT(godot_gdnative_init_options *options) {
}

/** GDNative Terminate **/
GDNATIVE_TERMINATE(godot_gdnative_terminate_options *options) {

}

/** NativeScript Initialize **/
NATIVESCRIPT_INIT() {
  register_class<SimpleClass>();
}
