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
    /*
    if(argc==2&&strcmp(argv[1],"client")==0)
    {


        std::cout<<"||===========This is client===========||"<<std::endl;
        ipc_sharedmemory test_req(IPC_MODE::MODE_REQUEST,"test");
        test_req.init("");

        IPC_DATA*defaule_data_= new IPC_DATA;
        defaule_data_->data="Default";
        ipc::write_memory(test_req.getSharedMemory(),defaule_data_);

        std::string request;



        std::cout<<"Respond from server: "<<test_req.req(request)<<std::endl;


    }
    else if(argc==2&&strcmp(argv[1],"server")==0)
    {
        std::cout<<"||===========This is server===========||"<<std::endl;
        ipc_sharedmemory test_rep(IPC_MODE::MODE_RESPOND,"test");
        test_rep.init("");
        std::string respond;

        std::cout<<"Please input respond: "<<std::endl;
        std::cin>>respond;
        std::cout<<"Request from clinet: "<<test_rep.rep_read()<<std::endl;
        test_rep.rep_write(respond);


    }
    */
    //=====================test for the speed of rep/req==================//

    if(argc==2&&strcmp(argv[1],"client")==0)
    {
        std::cout<<"||===========This is client===========||"<<std::endl;
        ipc_sharedmemory test_req(IPC_MODE::MODE_REQUEST,"test");
        test_req.init("");
        std::string request;

        std::cout<<"please input request: "<<std::endl;
        std::cin>>request;

        for(size_t i=0;i<1000;i++)
        {
            test_req.req(request);
        }

    }

    else if(argc==2&&strcmp(argv[1],"server")==0)
    {
        std::cout<<"||===========This is server===========||"<<std::endl;
        ipc_sharedmemory test_rep(IPC_MODE::MODE_RESPOND,"test");
        test_rep.init("");
        std::string respond;

        std::cout<<"Please input respond: "<<std::endl;
        std::cin>>respond;

        QTime test_time;
        test_time.start();
        for(size_t i=0;i<1000;i++)
        {
            test_rep.rep_write(respond);
        }
        int mscend=test_time.elapsed();
        std::cout<<"data_pack_size_ is:"<<sizeof(respond)<<std::endl;
        std::cout<<"use_time_ is :"<<mscend<<std::endl;

        std::cout<<"Data persecond: "<<1000000.*(float)sizeof(respond)/(float)mscend <<
                "Bytes!"<<std::endl;

        sizeof(respond);
    }
    else
    {
        std::string test;
        std::cin>>test;

        std::cout<<"Size is :"<<sizeof(test)<<std::endl;
    }






    //==========================test 01===================================//
    //delete ipc_data_read;
    //delete ipc_data_write;


    return a.exec();
}
