#include "app.h"

int main(int argc, char* argv[]) {
    App app;
    if (const int status = app.init()) {
        return status;
    }
    return app.run();
}
