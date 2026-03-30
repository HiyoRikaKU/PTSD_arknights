#include "Arknights/App.hpp"

#include "Core/Context.hpp"

int main(int, char**) {
    auto context = Core::Context::GetInstance();
    Arknights::App app;

    while (!context->GetExit()) {
        switch (app.getCurrentState()) {
            case Arknights::App::State::START:
                app.start();
                break;

            case Arknights::App::State::UPDATE:
                app.update();
                break;

            case Arknights::App::State::END:
                app.end();
                context->SetExit(true);
                break;
        }
        context->Update();
    }
    return 0;
}
