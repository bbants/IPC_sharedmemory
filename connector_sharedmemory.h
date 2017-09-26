#ifndef CONNECTOR_SHAREDMEMORY_H
#define CONNECTOR_SHAREDMEMORY_H

//#include <QtWidgets/QMainWindow>
#include <QObject>
#include "QSharedMemory"
#include "QBuffer"
#include "QDataStream"
#include "QTextStream"
#include <QThread>
#include "QtGlobal"
#include "QTime"
#include "QMap"
#include <QTimer>

#include "iostream"
#include "sstream"

namespace ipc {
    const std::string ipc_sharedmemory_type_name="ipc_sharedmemory";
    const std::string ipc_sharedmemory_id="100000";

    const double IPC_CHECK_DELAY=0.0001;
    const double WR_MEMORY_DELAY=0.0001;
    const double WR_MEMORY_TOLERANCE=1.0;
    const double SR_MESSAGE_DELAY=0.0001;
    const double SR_MESSAGE_TOLERANCE=1.0;

    enum class IPC_MODE
    {
        MODE_DEFAULT=100,
        MODE_REQUEST=101,
        MODE_RESPOND=102,
        MODE_SUBSCRIBE=105,
        MODE_PUBLISH=106
    };
    enum class COMMAND_MODE
    {
        COMMAND_DEFAULT=200,
        COMMAND_REQUEST=201,
        COMMAND_RESPONSE=202,
        COMMAND_RECIVE=203,

        COMMAND_REGISTER=204,
        COMMAND_REG_FINSHED=205,
        COMMAND_REG_FAILED=206
    };

    struct IPC_DATA
    {
        IPC_MODE ipc_mode_;
        COMMAND_MODE command_mode_;
        QString data;
        IPC_DATA():ipc_mode_(IPC_MODE::MODE_DEFAULT),command_mode_(COMMAND_MODE::COMMAND_DEFAULT),data(QString(""))
        {}
        void setValue(IPC_MODE ipc_mode_,COMMAND_MODE command_mode_,QString data)
        {
            this->ipc_mode_=ipc_mode_;
            this->command_mode_=command_mode_;
            this->data=data;
        }
        void print()
        {
            std::cout<<"<Read the IPC_DATA [start!]>"<<std::endl;
            std::cout<<"ipc_mode is: "<<(int)ipc_mode_<<"||"
                    <<"command_mode is: "<<(int)command_mode_<<std::endl
                   <<"data: "<<this->data.toStdString()<<std::endl
                  <<"<Read the IPC_DATA [finish!]>"<<std::endl<<std::endl;
        }
    };


    //===========write/read to/from memory============//
    bool write_memory(QSharedMemory*,IPC_DATA*);
    bool read_memory(QSharedMemory*,IPC_DATA*);
    bool check_command(QSharedMemory*,IPC_DATA*,COMMAND_MODE);
    bool send_message(QSharedMemory*,IPC_DATA*,COMMAND_MODE);
    bool recive_message(QSharedMemory*,IPC_DATA*,COMMAND_MODE);


    class ipc_sharedmemory:
            public QObject
    {
        Q_OBJECT

    public:
        ipc_sharedmemory(IPC_MODE connector_mode,const std::string connector_name="",QObject* parent=0);
        //ipc_sharedmemory(const ipc_sharedmemory & ipc);
        virtual ~ipc_sharedmemory(){}

        //void timerEvent(QTimerEvent *event) override;
    public:
        explicit ipc_sharedmemory(QObject *parent = nullptr);


    public:
        void init(const std::string& inin_str);
        //void test();
        std::string get_connector_name();
        IPC_MODE get_connector_mode();
        QSharedMemory* getSharedMemory();
    private:
        void check_mode(IPC_MODE mode);
        void check_port(std::string connector_name);
        void check_port_assist();

    public:
    //============================rep/req================================//
        std::string req(const std::string& request);

        void        rep_write(const std::string &respond);
        std::string rep_read();

    //===================================================================//


    //=============================sub/pub===============================//
        bool sub_register(std::string sub_name);
    public slots:
        void pub_register();

        bool pub_publish(std::string);
        bool check_pub_status();
        std::string sub_recive();
    //===================================================================//


    //================================test===============================//
        void pub_publish_test();
    //===================================================================//


    private:
        QSharedMemory* ipc_memory_;
        std::string   connector_name;
        IPC_MODE      connector_mode;

    private:
        QSharedMemory* ipc_sub_memory_;
        QTimer*        pub_register_timer;
        QTimer*        pub_publish_timer;
        QTimer*        sub_recive_timer;
        std::string    publish_data_toSend_;
        QMap<QString,QPair<QSharedMemory*,bool>> pub_server_status_;

    public:
        void setPublishData(std::string data)
        {
            this->publish_data_toSend_=data;
        }
        //===========================test===========================//
        /*
    public:
        QTimer*        test_timer_;
        void start_test_timer()
        {
            //test_timer_->start(100);
            pub_register_timer->start(1000);
        }
    public slots:
        inline void slotTimeOut(){
            static int k = 0;
            std::cout << "pulse: " << QString::number(k++).toStdString() << std::endl;
        }
        */
        //===========================test end==========================//




    };











}


#endif // CONNECTOR_SHAREDMEMORY_H
