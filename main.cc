#include <gtkmm.h>
#include "screenshot.h"
#include <string>
#include <iostream>
#include <fstream>
#include <glibmm.h>
#include <glibmm/optioncontext.h>

enum class Mode{
    fullclipboard,
    areaclipboard,
    fullsave,
    areasave
};

class ScreenshotApp: public Gtk::Application{
    protected:
        ScreenshotApp() : Gtk::Application("net.rainyseasons.screenshot"){};

        void on_activate() override {
            std::string geometry = run_command("slurp");
            if(geometry.empty())
                return;

            auto image_data = run_grim(geometry);

            if(image_data.empty()) 
                return;

            hold();
            auto dialog = Gtk::FileDialog::create(); // create a file dialog widget
            dialog->set_title("Save screenshot");
            dialog->set_initial_name("screenshot.png");

            dialog->save([this,dialog, image_data = std::move(image_data)](const Glib::RefPtr<Gio::AsyncResult>& result){
                std::cout << "Callback triggered\n";
                try {
                    auto file = dialog->save_finish(result);
                    std::cout << "save_finish returned\n";

                    if(!file){
                        return;
                    }

                    std::cout << "Selected path: " << file->get_path() << "\n";
                    auto path = file->get_uri();

                    if(path.empty()){
                        std::cout<<"file is empty!";
                    }

                    auto ostream = file->replace();
                    ostream->write(image_data.data(), image_data.size());
                    ostream->close();
                    
                    release();
                } catch (const Glib::Error& ex) {
                    std::cout << "Error: " << ex.what() << "\n";
                    release();
                }
            });

        }

    public:
    static Glib::RefPtr<ScreenshotApp> create() {
        return Glib::RefPtr<ScreenshotApp>(new ScreenshotApp());
    }
};

int main(int argc, char* argv[]){
    auto app = ScreenshotApp::create();
    return app->run(argc,argv);
}