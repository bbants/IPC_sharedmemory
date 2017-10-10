#include <QCoreApplication>
#include "connector_sharedmemory.h"


using namespace ipc;


int main(int argc, char *argv[])
{

    QCoreApplication a(argc, argv);

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
            std::cout<<".";
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
            std::cout<<".";
            test_rep.rep_write(respond);
        }
        int mscend=test_time.elapsed();
        std::cout<<"data_pack_size_ is:"<<sizeof(respond)<<std::endl;
        std::cout<<"use_time_ is :"<<mscend<<std::endl;

        std::cout<<"Data persecond: "<<1000000.*(float)respond.length()/(float)mscend <<
                "Bytes!"<<std::endl;

        sizeof(respond);
    }
    else if (argc==2&&strcmp(argv[1],"sub")==0)
    {
        std::cout<<"||===========This is client===========||"<<std::endl;
        ipc_sharedmemory* test_sub_=new ipc_sharedmemory(IPC_MODE::MODE_SUBSCRIBE,"test");
        test_sub_->init("");
        std::string sub_name_;

        std::cout<<"Please input sub_name: "<<std::endl;
        std::cin>>sub_name_;

        test_sub_->sub_register(sub_name_);

    }
    else if(argc==2&&strcmp(argv[1],"pub")==0)
    {
         std::cout<<"||===========This is server===========||"<<std::endl;
         ipc_sharedmemory* test_pub=new ipc_sharedmemory(IPC_MODE::MODE_PUBLISH,"test",&a);
         test_pub->setPublishData("a test");
         test_pub->init("");

         std::string test_data_string_;

         /*
         std::cout<<"publish data:"<<std::endl;
         std::cin>>test_data_string_;
         test_pub->pub_publish(test_data_string_);
         */
         //test_pub->start_test_timer();
         //test_pub->start_test_timer();
         //std::string test_data_string_;

         //for(size_t i=0;i<10;i++)
         //{
         //test_pub->pub_publish();
         //}
    }
    else
    {
        /*
        QTimer t;
        QObject::connect(&t, &QTimer::timeout, [&]()
        {
            std::cout<<"aaaaaaa";
            a.exit();
        });
        t.start(1000);
        */
        /*
        std::cout<<"||===========This is server===========||"<<std::endl;
        ipc_sharedmemory test_pub(IPC_MODE::MODE_PUBLISH,"test");
        test_pub.init("");
        test_pub.test_timer();
        test_pub.
        */

        //std::cout<<"||===========This is client===========||"<<std::endl;
        //ipc_sharedmemory* test_sub_=new ipc_sharedmemory(IPC_MODE::MODE_SUBSCRIBE,"test",&a);
        //test_sub_.init("");
        //test_sub_->start_test_timer();

        //TimerTest* tt = new TimerTest(&a);
        //tt->start();
        IPC_DATA* test_data_;
        test_data_=new IPC_DATA;
        QSharedMemory* test;
        test=new QSharedMemory;
        test->setKey("test");
        read_memory(test,test_data_);
        test_data_->print();
    }






    //==========================test 01===================================//
    //delete ipc_data_read;
    //delete ipc_data_write;


    return a.exec();
}
