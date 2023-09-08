#include <iostream>
#include <thread>
#include <memory>

#include <Queue/EventQueue.hpp>
#include <Devices/DeviceA.hpp>
#include <Devices/DeviceB.hpp>
#include <Events/StartedEvent.hpp>
#include <Events/DataEvent.hpp>
#include <Events/WorkDoneEvent.hpp>

//функция, которая позволяет протестирвоать работу в следующий случаях:
//оба устройства работают в штатном режиме,
//устройство А перестает отвечать через заданное число вызовов функции readA, устройство В работает в штатном режиме
//устройство В перестает отвечать через заданное число вызовов функции readВ, устройство А работает в штатном режиме,
//оба устройства перестают отвечать каждое через заданное число вызовов соответствующих функций.

void differentModes(bool normalA, bool normalB) { 
    //количество вызовов чтения с устройств
    size_t numOfCallsA = 5;
    size_t numOfCallsB = 5;

    //заданное количество вызовов функции read, после которого устройство перестаёт отвечать
    size_t numForErrorA = numOfCallsA - numOfCallsA / 2;
    size_t numForErrorB = numOfCallsB - numOfCallsB / 2;

    //очередь сообщений
    EventQueue eventQueue;

    // устройства А и В
    std::shared_ptr<Device> deviceA = std::make_shared<DeviceA>();
    std::shared_ptr<Device> deviceB = std::make_shared<DeviceB>();

    size_t cntA = 0;
    size_t cntB = 0;

    //закончилось ли считывание устройств без ошибок
    bool workDoneA = false;
    bool workDoneB = false;

    //поток для устройства А
    std::thread threadA([&]() {
        std::shared_ptr<StartedEvent> startedEvent = std::make_shared<StartedEvent>(deviceA);
        eventQueue.push(startedEvent); //добавляем событие начала считывания в очередь
        while (cntA < numOfCallsA) { 
            //имитируем отсутствие ответа при чтении с утройства А через заданное количество вызовов функции read при необходимости
            if (!normalA && cntA == numForErrorA) {
                std::this_thread::sleep_for(std::chrono::seconds(10000));
            }
            
            deviceA->read(); //читаем с устройства
            std::shared_ptr<DataEvent> dataEvent = std::make_shared<DataEvent>(deviceA);
            eventQueue.push(dataEvent); //добавляем данные, считанные с устройства, в очередь
            ++cntA;
        }
        std::shared_ptr<WorkDoneEvent> workDoneEvent = std::make_shared<WorkDoneEvent>(deviceA);
        eventQueue.push(workDoneEvent); //в случае, если всё успешно считалось, добавлемя событие окончания считывания в очередь
        workDoneA = true; //обозначаем, что всё успешно выполнилось
        });


    //поток для устройства B
    std::thread threadB([&]() {
        std::shared_ptr<StartedEvent> startedEvent = std::make_shared<StartedEvent>(deviceB);
        eventQueue.push(startedEvent);
        while (cntB < numOfCallsB) {
            //имитируем отсутствие ответа при чтении с утройства B через заданное количество вызовов функции read
            if (!normalB && cntB == numForErrorB) {
                std::this_thread::sleep_for(std::chrono::seconds(10000));
            }

            deviceB->read();
            std::shared_ptr<DataEvent> dataEvent = std::make_shared<DataEvent>(deviceB);
            eventQueue.push(dataEvent);
            ++cntB;
        }
        std::shared_ptr<WorkDoneEvent> workDoneEvent = std::make_shared<WorkDoneEvent>(deviceB);
        eventQueue.push(workDoneEvent);
        workDoneB = true;
        });

    // Main поток
    while (true) {
        //переменная для понимания, что устройство перестало отвечать и отправлять сообщения
        bool checkWaiting = true;
        
        size_t maxWaiting; //переменная наибольшей задержки у двух устройств для определения, что действительно больше нет ответа
        if (deviceA->returnDelay() >= deviceB->returnDelay())
            maxWaiting = deviceA->returnDelay() + 1;
        else
            maxWaiting = deviceB->returnDelay() + 1;

        std::shared_ptr<const Event> event = eventQueue.pop(std::chrono::seconds(maxWaiting), checkWaiting);
        if (event != nullptr) {
            std::cout << event->toString() << std::endl;
        }

        //проверка на то, что завершился приём данных и обработка информации
        if (workDoneA && workDoneB) { //успешная работа
            std::cout << "Successful completion of reading and processing of information from two devices" << std::endl;
            threadA.detach();
            threadB.detach();
            break;
        }
        else {
            if (!checkWaiting) {
                if (workDoneA || workDoneB) { // одно из устройств перестало отвечать, но работа завершилась успешно
                    std::cout << "One device stopped responding, but the job was completed successfully" << std::endl;
                    threadA.detach();
                    threadB.detach(); 
                    break;
                }
                else {
                    if (!workDoneA && !workDoneB) { //оба устройства перестали отвечать
                        std::cout << "Both devices stopped responding" << std::endl;
                        threadA.detach();
                        threadB.detach();
                        break;
                    }
                    else { // одно из устройств перестало отвечать и работа завершилась не успешно
                        std::cout << "One device stopped responding and an error occurred" << std::endl;
                        threadA.detach();
                        threadB.detach();
                        break;
                    }
                }
            }
        }
    }
}


int main() {
     //переменные для проверки корректности работы программы при сбоях чтения с устройств
    bool normalA; //если false, то проверяяется корректность выполнения программы при сбое чтения с устройства А
    bool normalB; //аналогично для устройства B

    std::cout << "Select a mode: \nRegular mode - press '1' \nDevice A is working normally, B is not - press '2' \n";
    std::cout << "Device B is working normally, A is not - press '3' \n";
    std::cout << "Both devices are not working in normal mode - press '4'" << std::endl;

    char check;
    std::cin >> check;

    switch (check) {
    case '1': 
        normalA = true;
        normalB = true;
        differentModes(normalA, normalB);
        break;
    case '2':
        normalA = true;
        normalB = false;
        differentModes(normalA, normalB);
        break;
    case '3':
        normalA = false;
        normalB = true;
        differentModes(normalA, normalB);
        break;
    case '4':
        normalA = false;
        normalB = false;
        differentModes(normalA, normalB);
        break;
    }
    
    system("pause");
    return 0;
}
