#include "screenshot.h"
#include "util.h"
#include <gtkmm.h>


// protected
ScreenshotApp::ScreenshotApp() : Gtk::Application(APP_NAME){};

void ScreenshotApp::on_activate() override {
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

void ScreenshotApp::notify(const std::string& title, const std::string& body) // has to be inside class definition
{
    auto notification = Gio::Notification::create(title);
    notification->set_body(body);
    notification->set_icon(Gio::ThemedIcon::create("image-x-generic-symbolic"));
        
    send_notification(APP_NAME, notification);
}


std::vector<unsigned char> ScreenshotApp::getImage(AreaType type){

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

void ScreenshotApp::saveFile(std::vector<unsigned char>& image_data){
  
    hold();
    auto dialog = Gtk::FileDialog::create(); // create a file dialog widget
    dialog->set_title("Save screenshot");
    dialog->set_initial_name("screenshot.png");
  
    dialog->save([this,dialog, image_data = std::move(image_data)](const Glib::RefPtr<Gio::AsyncResult>& result){
        std::cout << "Callback triggered\n";
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


void ScreenshotApp::copyClipboard(std::vector<unsigned char>& image_data){
    FILE* pipe = popen("wl-copy", "w");
    fwrite(image_data.data(), 1, image_data.size(), pipe);
    pclose(pipe);
    notify("Screenshot taken!", "Copied to clipboard");
}


// public:
static ScreenshotApp::Glib::RefPtr<ScreenshotApp> create() {
    return Glib::RefPtr<ScreenshotApp>(new ScreenshotApp());
}
void ScreenshotApp::setAreaType(const AreaType& type){
    area_type = type;
}

void ScreenshotApp::setSaveType(const SaveType& type){
    save_type = type;
}