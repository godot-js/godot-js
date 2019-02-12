#ifndef GDJavaScript_H
#define GDJavaScript_H

#include <Godot.hpp>
#include <Sprite.hpp>

namespace godot {

class GDJavaScript : public Sprite {
    GODOT_CLASS(GDJavaScript, Sprite)

   private:
   public:
    static void _register_methods();

    GDJavaScript();
    ~GDJavaScript();

    void _init();  // our initializer called by Godot
    void _process(float delta);
};

class GodotProxy {

};

}  // namespace godot

#endif
