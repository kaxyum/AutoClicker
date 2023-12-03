#include <windows.h>
#include <SFML/Graphics.hpp>
#include <filesystem>
#include <thread>
#include <atomic>
#include "SliderSFML.h"
#include <chrono>

bool isKeyPressed(int key) {
    return GetAsyncKeyState(key) & 0x8000;
}

void autoClicker(std::atomic<int>& limit, std::atomic<bool>& on) {
    while (true) {
        if (on.load()) {
            auto start_time = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < limit.load(); i++) {
                Sleep(5);
                POINT p;
                GetCursorPos(&p);

                mouse_event(MOUSEEVENTF_LEFTDOWN, p.x, p.y, 0, 0);
                mouse_event(MOUSEEVENTF_LEFTUP, p.x, p.y, 0, 0);
            }
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
            Sleep(1000 - duration.count());
        }
    }
}

int main() {
    std::atomic<int> limit(15);
    std::atomic<bool> on(false);

    sf::RenderWindow window(sf::VideoMode(450, 200), "AutoClicker by KaxYum", sf::Style::Titlebar | sf::Style::Close);

    sf::Font font;
    std::filesystem::path fontPath = std::filesystem::current_path();
    fontPath.append("ARIAL.TTF");
    if (!font.loadFromFile(fontPath.string())) {
        return -1;
    }

    sf::Text text("PRESS X FOR ON/OFF THE AUTOCLICKER", font, 20);
    text.setFillColor(sf::Color::White);
    text.setPosition(25.f, 50.f);

    sf::Text fpsText("cps per second", font, 20);
    fpsText.setFillColor(sf::Color::White);
    fpsText.setPosition(142.5f, 150.f);

    SliderSFML slider(115, 120);

    slider.create(0, 50);

    std::thread autoClickThread(autoClicker, std::ref(limit), std::ref(on));

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        window.clear(sf::Color(128, 128, 128));

        slider.draw(window);

        window.draw(text);
        window.draw(fpsText);
        window.display();

        limit.store(slider.getSliderValue());

        if (isKeyPressed(0x58)) {
            on.store(!on.load());
        }
    }

    autoClickThread.join();

    return 0;
}