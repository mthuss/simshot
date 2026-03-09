#include "screenshot.h"
#include <gtkmm.h>
#include <string>
#include <vector>
#include <cstdio>

class ScreenshotApp: public Gtk::Application{
    AreaType area_type = AreaType::Full;
    SaveType save_type = SaveType::File;
    public:
        static Glib::RefPtr<ScreenshotApp> create();
        void setAreaType(const AreaType& type);
        void setSaveType(const SaveType& type);

    protected:
        // ScreenshotApp() : Gtk::Application(APP_NAME){};
        void on_activate() override;
        void notify(const std::string& title, const std::string& body);
        std::vector<unsigned char> getImage(AreaType type);
        void saveFile(std::vector<unsigned char>& image_data);
        void copyClipboard(std::vector<unsigned char>& image_data);
