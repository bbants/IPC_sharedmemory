#include <QCoreApplication>
#include "connector_sharedmemory.h"

using namespace ipc;


int main(int argc, char *argv[])
{

    QCoreApplication a(argc, argv);
    /*
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
    */
    //============================test for req/rep==========================//
    if(argc==2&&atoi(argv[1])==(int)IPC_MODE::MODE_REQUEST)
    {


        std::cout<<"||===========This is client===========||"<<std::endl;
        ipc_sharedmemory test_req(IPC_MODE::MODE_REQUEST,"test");
        test_req.init("");

        IPC_DATA*defaule_data_= new IPC_DATA;
        defaule_data_->data="Default";
        ipc::write_memory(test_req.getSharedMemory(),defaule_data_);

        std::string request;
        /*
        while(true)
        {
            std::cout<<"please input request: "<<std::endl;
            std::cin>>request;
            if(request.compare("exit")==0)
            {
                break;
            }
            std::cout<<test_req.req(request)<<std::endl;

        }
        */
        std::cout<<"please input request: "<<std::endl;
        std::cin>>request;
        test_req.req(request);

    }
    else if(argc==2&&atoi(argv[1])==(int)IPC_MODE::MODE_RESPOND)
    {
        std::cout<<"||===========This is server===========||"<<std::endl;
        ipc_sharedmemory test_rep(IPC_MODE::MODE_RESPOND,"test");
        test_rep.init("");
        std::string respond;
        std::cout<<"Please input respond"<<std::endl;
        /*
        while(true)
        {
            std::cout<<test_rep.rep_read();
            test_rep.rep_write("This is respond!!!!!!!!!!");
        }
        */
        std::cin>>respond;
        std::cout<<test_rep.rep_read();
        test_rep.rep_write(respond);
    }

    //delete ipc_data_read;
    //delete ipc_data_write;


    return a.exec();
}
