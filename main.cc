#include <gtkmm.h>
#include "screenshot.h"
// #include "util.h"
#include <string>
#include <iostream>
#include <glibmm.h>
#include <glibmm/optioncontext.h>
#include <vector>
#define APP_NAME "net.rainyseasons.screenshot"


class ScreenshotApp: public Gtk::Application{
    protected:
        AreaType area_type = AreaType::Full;
        SaveType save_type = SaveType::File;

        ScreenshotApp() : Gtk::Application(APP_NAME){};

        void on_activate() override {
            auto image_data = getImage(area_type);

            if(image_data.empty())
                return;


            if(save_type == SaveType::File)
                saveFile(image_data);
            else if(save_type == SaveType::Clipboard)
                copyClipboard(image_data);
            else{
                std::cout << "Uh oh...\n";
                return;
            }
        }

        void notify(const std::string& title, const std::string& body) // has to be inside class definition
        {
            auto notification = Gio::Notification::create(title);
            notification->set_body(body);
            notification->set_icon(Gio::ThemedIcon::create("image-x-generic-symbolic"));
        
            send_notification(APP_NAME, notification);
        }

        std::vector<unsigned char> getImage(AreaType type){

            if(type == AreaType::Selection){
                std::cout << "Taking screenshot of selected area\n";
                std::string geometry = run_command("slurp");
                if(geometry.empty())
                    return std::vector<unsigned char>();
  
                auto image_data = run_grim(geometry);
                if(image_data.empty()) 
                    return std::vector<unsigned char>();

                return image_data;
            }
            else if(type == AreaType::Full){
                std::cout << "Taking fullscreen screenshot\n";
                auto image_data = run_grim();
                if(image_data.empty()) 
                    return std::vector<unsigned char>();
                return image_data;
            }
        }
        void saveFile(std::vector<unsigned char>& image_data){
  
            hold();
            auto dialog = Gtk::FileDialog::create(); // create a file dialog widget
            dialog->set_title("Save screenshot");
            dialog->set_initial_name("screenshot.png");
  
            dialog->save([this,dialog, image_data = std::move(image_data)](const Glib::RefPtr<Gio::AsyncResult>& result){
                // std::cout << "Callback triggered\n";
                try {
                    auto file = dialog->save_finish(result);
  
                    if(!file){
                        return;
                    }
  
                    // std::cout << "Selected path: " << file->get_path() << "\n";
                    auto path = file->get_path();

  
                    if(path.empty()){
                        std::cout<<"file is empty!";
                    }
  
                    auto ostream = file->replace();
                    ostream->write(image_data.data(), image_data.size());
                    ostream->close();

                    notify("Screenshot taken!", "Saved to " + path);

                    release();
                } catch (const Glib::Error& ex) {
                    std::cout << "Error: " << ex.what() << "\n";
                    release();
                }
            });
        }

        void copyClipboard(std::vector<unsigned char>& image_data){
            FILE* pipe = popen("wl-copy", "w");
            fwrite(image_data.data(), 1, image_data.size(), pipe);
            pclose(pipe);
            notify("Screenshot taken!", "Copied to clipboard");
        }
    public:
    static Glib::RefPtr<ScreenshotApp> create() {
        return Glib::RefPtr<ScreenshotApp>(new ScreenshotApp());
    }
    void setAreaType(const AreaType& type){
        area_type = type;
    }

    void setSaveType(const SaveType& type){
        save_type = type;
    }
};

int main(int argc, char* argv[]){
    bool opt_full = false;
    bool opt_select = false;
    bool opt_save = false;
    bool opt_clip = false;
            
    Glib::OptionEntry full;
    full.set_long_name("full");
    full.set_short_name('f');
    full.set_description("Take full screenshot");
            
    
    Glib::OptionEntry sel;
    sel.set_long_name("selection");
    sel.set_short_name('s');
    sel.set_description("Select screenshot area");

    Glib::OptionEntry save;
    save.set_long_name("save");
    save.set_short_name('S');
    save.set_description("Save to file");

    Glib::OptionEntry clip;
    clip.set_long_name("clip");
    clip.set_short_name('C');
    clip.set_description("Copy to clipboard");
            
    Glib::OptionGroup group("main", "Main Options");
    group.add_entry(full, opt_full);
    group.add_entry(sel, opt_select);
    group.add_entry(save, opt_save);
    group.add_entry(clip, opt_clip);
            
    Glib::OptionContext context;
    context.set_main_group(group);
            
    try {
        context.parse(argc, argv);
    }
    catch(const Glib::Error& ex) {
        std::cerr << ex.what() << std::endl;
        return 1;
    }

    if(opt_full && opt_select) {
        std::cerr << "Cannot use --full and --select together\n";
        return 1;
    }

    if(opt_save && opt_clip){
        std::cerr << "Cannot use --save and --clip together\n";
        return 1;
    }


    auto app = ScreenshotApp::create();

    if(opt_full)
        app->setAreaType(AreaType::Full);
    else if(opt_select)
        app->setAreaType(AreaType::Selection);

    if(opt_save)
        app->setSaveType(SaveType::File);
    else if(opt_clip)
        app->setSaveType(SaveType::Clipboard);

    return app->run(argc,argv);
}
