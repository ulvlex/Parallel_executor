#include <Queue/EventQueue.hpp>
#include <Devices/DeviceA.hpp>
#include <Devices/DeviceB.hpp>
#include <Events/StartedEvent.hpp>
#include <Events/DataEvent.hpp>
#include <Events/WorkDoneEvent.hpp>

#include <boost/program_options.hpp>

#include <iostream>
#include <thread>
#include <memory>

namespace po = boost::program_options;


//функция, которая позволяет протестировать работу в следующий случаях:
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
        eventQueue.push(std::make_shared<StartedEvent>(deviceA)); //добавляем событие начала считывания в очередь
        while (cntA < numOfCallsA) { 
            //имитируем отсутствие ответа при чтении с утройства А через заданное количество вызовов функции read при необходимости
            if (!normalA && cntA == numForErrorA) {
                std::this_thread::sleep_for(std::chrono::seconds(10000));
            }
            
            deviceA->read(); //читаем с устройства
            eventQueue.push(std::make_shared<DataEvent>(deviceA)); //добавляем данные, считанные с устройства, в очередь
            ++cntA;
        }
        eventQueue.push(std::make_shared<WorkDoneEvent>(deviceA)); //в случае, если всё успешно считалось, добавлемя событие окончания считывания в очередь
        workDoneA = true; //обозначаем, что всё успешно выполнилось
        });


    //поток для устройства B
    std::thread threadB([&]() {
        eventQueue.push(std::make_shared<StartedEvent>(deviceB));
        while (cntB < numOfCallsB) {
            //имитируем отсутствие ответа при чтении с утройства B через заданное количество вызовов функции read
            if (!normalB && cntB == numForErrorB) {
                std::this_thread::sleep_for(std::chrono::seconds(10000));
            }

            deviceB->read();
            eventQueue.push(std::make_shared<DataEvent>(deviceB));
            ++cntB;
        }
        eventQueue.push(std::make_shared<WorkDoneEvent>(deviceB));
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

        std::shared_ptr<const Event> event = eventQueue.pop(std::chrono::seconds(maxWaiting));
        if (event != nullptr) {
            std::cout << event->toString() << std::endl;
        }
        else {
            checkWaiting = false;
        }

        //проверка на то, что завершился приём данных и обработка информации
        if (workDoneA && workDoneB) { //успешная работа
            std::cout << "Successful completion of reading and processing of information from two devices" << std::endl;
            threadA.detach();
            threadB.detach();
            break;
        }
        else  if (!checkWaiting) {
            if (workDoneA || workDoneB) { // одно из устройств перестало отвечать, но работа завершилась успешно
                std::cout << "One device stopped responding, but the job was completed successfully" << std::endl;
            }
            else {
                if (!workDoneA && !workDoneB) { //оба устройства перестали отвечать
                    std::cout << "Both devices stopped responding" << std::endl;
                }
                else { // одно из устройств перестало отвечать и работа завершилась не успешно
                    std::cout << "One device stopped responding and an error occurred" << std::endl;
                }
            }

            threadA.detach();
            threadB.detach();
            break;
        }
    }
}


int main(int argc, char** argv) {
    //переменные для проверки корректности работы программы при сбоях чтения с устройств
    bool normalA; //если false, то проверяяется корректность выполнения программы при сбое чтения с устройства А
    bool normalB; //аналогично для устройства B

    po::options_description desc("Options");

    desc.add_options()
        ("help", "Print help messages") //Вызов help справки
        ("regular-mode, rm", "choosing a regular mode")
        ("stop-b, sb", "device A is working normally, B is not")
        ("stop-a, sa", "device B is working normally, A is not")
        ("stop-both, s", "both devices are not working in normal mode");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);  // парсим переданные аргументы
    po::notify(vm); // записываем аргументы в переменные в программе

    // Если есть запрос на справку
    if (vm.count("help"))
    {
        // То выводим описание меню
        std::cout << desc << std::endl;
        return 1;
    }

    if (vm.count("regular-mode"))
    {
        normalA = true;
        normalB = true;
    }
    else if (vm.count("stop-b"))
    {
        normalA = true;
        normalB = false;
    }
    else if (vm.count("stop-a")) {
        normalA = true;
        normalB = false;
    } 
    else if (vm.count("stop-both"))
    {
        normalA = false;
        normalB = false;
    }
    else {
        std::cout << "The parameter is entered incorrectly, please refer to the help." << std::endl;
        return -1;
    }
    differentModes(normalA, normalB);
    
    std::cin.get();
    return 0;
}
