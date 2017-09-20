#include <QCoreApplication>
#include "connector_sharedmemory.h"

using namespace ipc;


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    //==========unit test==========//
    ipc_sharedmemory test_req(IPC_MODE::MODE_REQUEST,"test");
    test_req.init("");
    ipc_sharedmemory test_rep(IPC_MODE::MODE_RESPOND,"test");
    test_rep.init("");

    IPC_DATA* ipc_data_write=new IPC_DATA;
    ipc_data_write->data="If you can see this, write and read is successful!";
    IPC_DATA* ipc_data_read=new IPC_DATA;

    write_memory(test_req.getSharedMemory(),ipc_data_write);
    ipc_data_write->data="If you can see this, send and recive message is successful!";
    send_message(test_req.getSharedMemory(),ipc_data_write,COMMAND_MODE::COMMAND_DEFAULT);
    //std::cout<<"start to recive!!!!!!!!!!"<<std::endl;

    recive_message(test_rep.getSharedMemory(),ipc_data_read,COMMAND_MODE::COMMAND_DEFAULT);

    //std::cout<<"finish recive!!!!!!!!!!"<<std::endl;

    delete ipc_data_read;
    delete ipc_data_write;


    return a.exec();
}
