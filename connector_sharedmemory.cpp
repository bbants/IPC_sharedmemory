#include "connector_sharedmemory.h"





ipc::ipc_sharedmemory::ipc_sharedmemory(IPC_MODE connector_mode,const std::string connector_name,QObject* parent)
    :QObject(parent)
    ,pub_register_timer(new QTimer(parent))
    ,test_timer_(new QTimer(parent))

{
    this->connector_name=connector_name;
    this->connector_mode=connector_mode;
    std::cout<<"connector_mode is:"<<(int)this->connector_mode<<std::endl;
    std::cout<<"connector_name is:"<<this->connector_name<<std::endl;
    /*
    connect(test_timer_
            ,SIGNAL(timeout())
            ,this
            ,SLOT(slotTimeOut())
            );
    */
    connect(pub_register_timer
            ,SIGNAL(timeout())
            ,this
            ,SLOT(pub_register())
            );

    //startTimer(1000);
}

/*
ipc::ipc_sharedmemory::ipc_sharedmemory(const ipc_sharedmemory & ipc)
{
    connector_name=ipc.get_connector_name();
    connector_mode=ipc.get_connector_mode();
}
*/
/*
void ipc::ipc_sharedmemory::timerEvent(QTimerEvent *event)
{
    pub_register();
}
*/

std::string ipc::ipc_sharedmemory::get_connector_name()
{
    return this->connector_name;
}
ipc::IPC_MODE ipc::ipc_sharedmemory::get_connector_mode()
{
    return this->connector_mode;
}
QSharedMemory* ipc::ipc_sharedmemory::getSharedMemory()
{
    return ipc_memory_;
}

void ipc::ipc_sharedmemory::check_mode(IPC_MODE ipc_mode)
{
    if(this->connector_mode!=ipc_mode)
    {
        std::cout<<"<Wrong connector mode>"<<std::endl;
    }
}
void ipc::ipc_sharedmemory::check_port(std::string connector_name)
{

    if(ipc_memory_->key().toStdString().compare(connector_name)!=0)
    {
        std::stringstream ss;
        ss<<"Wrong connector name";
         std::cout<<ss.str();
    }
}

void ipc::ipc_sharedmemory::init(const std::string& inin_str)
{
    ipc_memory_=new QSharedMemory;
    switch(this->connector_mode)
    {
        case IPC_MODE::MODE_REQUEST:
        ipc_memory_->setKey(QString::fromStdString(connector_name));
        break;

        case IPC_MODE::MODE_RESPOND:
        ipc_memory_->setKey(QString::fromStdString(connector_name));
        break;

        case IPC_MODE::MODE_SUBSCRIBE:
        ipc_memory_->setKey(QString::fromStdString(connector_name));
        break;

        case IPC_MODE::MODE_PUBLISH:
        ipc_memory_->setKey(QString::fromStdString(connector_name));
        //pub_register_timer=new QTimer(this);
        //QTimer::singleShot(200,this,SLOT(pub_register()));
        break;
    }
    IPC_DATA*defaule_data_= new IPC_DATA;
    ipc::write_memory(ipc_memory_,defaule_data_);
}

bool ipc::ipc_sharedmemory::sub_register(std::string sub_name_)
{

    check_mode(IPC_MODE::MODE_SUBSCRIBE);
    IPC_DATA* register_data_=new IPC_DATA;
    register_data_->setValue(IPC_MODE::MODE_SUBSCRIBE,COMMAND_MODE::COMMAND_REGISTER,QString::fromStdString(sub_name_));
    while(true)
    {
        std::cout<<"Send register command..."<<std::endl;
        if(ipc::send_message(ipc_memory_,register_data_,COMMAND_MODE::COMMAND_DEFAULT))
        {
            break;
        }
        else if(ipc::send_message(ipc_memory_,register_data_,COMMAND_MODE::COMMAND_REG_FINSHED))
        {
            break;
        }

    }
    while(true)
    {
        if(ipc::recive_message(ipc_memory_,register_data_,COMMAND_MODE::COMMAND_REG_FINSHED))
        {
            std::cout<<"Register as: "<<register_data_->data.toStdString()<<std::endl;
            ipc_sub_memory_=new QSharedMemory;
            ipc_sub_memory_->setKey(QString::fromStdString(sub_name_));
            break;
        }
        else if(ipc::recive_message(ipc_memory_,register_data_,COMMAND_MODE::COMMAND_REG_FAILED))
        {
            std::cout<<"Register faild: "<<sub_name_<<"is already exist!"<<std::endl;
            return false;
        }
    }
    return true;
}
//
//use Timer to drive pub_register pub_write
//
void ipc::ipc_sharedmemory::pub_register()
{
    //std::cout<<"bengkui!"<<std::endl;

    check_mode(IPC_MODE::MODE_PUBLISH);
    IPC_DATA* register_data_=new IPC_DATA;
    std::cout<<".";
    if(ipc::recive_message(ipc_memory_,register_data_,COMMAND_MODE::COMMAND_REGISTER))
    {
        std::cout<<std::endl<<"recive register command: "<<std::endl;
        QString sub_register_name_=register_data_->data;
        //register_data_->print();
        //std::cout<<sub_register_name_.toStdString()<<std::endl;
        if(pub_server_status_.contains(sub_register_name_))
        {
            register_data_->command_mode_=COMMAND_MODE::COMMAND_REG_FAILED;
            ipc::send_message(ipc_memory_,register_data_,COMMAND_MODE::COMMAND_REGISTER);
            register_data_->print();
            std::cout<<"Name: "<<sub_register_name_.toStdString()<<" is already exist, try again!"<<std::endl;
            //return false;
        }
        else
        {
            ipc_sub_memory_=new QSharedMemory;
            ipc_sub_memory_->setKey(sub_register_name_);
            bool message_recived_=false;

            pub_server_status_[sub_register_name_]=qMakePair(ipc_sub_memory_,message_recived_);
            register_data_->command_mode_=COMMAND_MODE::COMMAND_REG_FINSHED;
            ipc::send_message(ipc_memory_,register_data_,COMMAND_MODE::COMMAND_REGISTER);
            std::cout<<"Register a sub_client as: "<<sub_register_name_.toStdString()<<std::endl;
        }
    }

    //return true;
}


std::string ipc::ipc_sharedmemory::req(const std::string& request)
{
    check_mode(IPC_MODE::MODE_REQUEST);
    IPC_DATA* req_data_=new IPC_DATA;
    req_data_->setValue(IPC_MODE::MODE_REQUEST,COMMAND_MODE::COMMAND_REQUEST,QString::fromStdString(request));

    while(true)
    {
        //std::cout<<std::endl<<std::endl;
        //std::cout<<"Write into memory [start]"<<std::endl;
        if(ipc::send_message(ipc_memory_,req_data_,COMMAND_MODE::COMMAND_DEFAULT))
        {
               break;

        }
        else if(ipc::send_message(ipc_memory_,req_data_,COMMAND_MODE::COMMAND_RECIVE))
        {
               break;
        }
        //QThread::sleep(SR_MESSAGE_DELAY);
    }
    while(true)
    {
        //std::cout<<"Wait for respond [start]"<<std::endl;

        if(ipc::recive_message(ipc_memory_,req_data_,COMMAND_MODE::COMMAND_RESPONSE))
        {

            while(true)
            {
                //std::cout<<"Set memory default [start]"<<std::endl;
                req_data_->command_mode_=COMMAND_MODE::COMMAND_DEFAULT;
                if(ipc::send_message(ipc_memory_,req_data_,COMMAND_MODE::COMMAND_RESPONSE))
                {
                       break;
                }
                //QThread::sleep(SR_MESSAGE_DELAY);
            }
            break;
        }
        //QThread::sleep(SR_MESSAGE_DELAY);
    }



    std::string recive_data_=req_data_->data.toStdString();

    delete req_data_;
    return recive_data_;
}
void ipc::ipc_sharedmemory::rep_write(const std::string &respond)
{
    check_mode(IPC_MODE::MODE_RESPOND);
    IPC_DATA* rep_data_=new IPC_DATA;
    rep_data_->setValue(IPC_MODE::MODE_RESPOND,COMMAND_MODE::COMMAND_RESPONSE,QString::fromStdString(respond));

    while(true)
    {
        if(ipc::send_message(ipc_memory_,rep_data_,COMMAND_MODE::COMMAND_REQUEST))
        {
            break;
        }
        //QThread::sleep(SR_MESSAGE_DELAY);
    }
    delete rep_data_;
}
std::string ipc::ipc_sharedmemory::rep_read()
{
    check_mode(IPC_MODE::MODE_RESPOND);
    IPC_DATA* rep_data_=new IPC_DATA;
    rep_data_->setValue(IPC_MODE::MODE_RESPOND,COMMAND_MODE::COMMAND_RESPONSE,QString::fromStdString(""));

    while(true)
    {
        if(ipc::recive_message(ipc_memory_,rep_data_,COMMAND_MODE::COMMAND_REQUEST))
        {
            break;
        }
        //QThread::sleep(SR_MESSAGE_DELAY);
    }
    std::string recive_data_=rep_data_->data.toStdString();
    delete rep_data_;
    return recive_data_;
}

    //
    //
    //  Use the sharedMemory to write/read shared memory
    //  Only care about write/read data, dont care MQ mode
    //
    //
inline bool ipc::write_memory(QSharedMemory* qSharedMemory,IPC_DATA* ipc_data_)
{
    if(!qSharedMemory->isAttached())
    {
        qSharedMemory->attach();
    }

    //==================write to buffer=================//
    QBuffer buffer_temp_;
    buffer_temp_.open(QBuffer::ReadWrite);
    QDataStream write(&buffer_temp_);
    /*
    QString memory=QString("/%1/%2/%3/").
            arg((int)ipc_data_->ipc_mode_).
            arg((int)ipc_data_->command_mode_).
            arg(ipc_data_->data)
            ;
    write<<memory;
    */
    QMap <QString,QString>mempry_map_;
    mempry_map_["IPC_MODE"]=QString::number((int)ipc_data_->ipc_mode_,10);
    mempry_map_["COMMAND_MODE"]=QString::number((int)ipc_data_->command_mode_);
    mempry_map_["DATA"]=ipc_data_->data;
    write<<mempry_map_;


    int data_size=buffer_temp_.size();

    if(data_size==0)
    {
        std::stringstream ss;
        ss<<"Cant load data,check the message";
        std::cout<<ss.str();
        return false;
    }

    //============write to shared memory===============//
    if(qSharedMemory->size()==0)
    {
        if(!qSharedMemory->create(data_size))
        {
            //qSharedMemory->destroyed();
            std::cout<<"Cant create sharedMemory"<<std::endl;
        }
    }
    qSharedMemory->lock();
    char *to = static_cast<char *>(qSharedMemory->data());
    const char *from = buffer_temp_.data().data();
    memcpy(to, from, qMin((int)qSharedMemory->size(),(int)buffer_temp_.size()));
    //std::cout<<"Sharedmemory is created, size is:"<<qSharedMemory->size()<<std::endl;
    qSharedMemory->unlock();

    return true;
}

inline bool ipc::read_memory(QSharedMemory* qSharedMemory,IPC_DATA* ipc_data_)
{

    QTime sr_time_;
    //std::cout<<"Read memory..."<<std::endl;
    sr_time_.start();

    if(!qSharedMemory->isAttached())
    {
        qSharedMemory->attach();
    }


    //=============read the memory=====================//
    QBuffer buffer_temp_;
    QDataStream read(&buffer_temp_);

    qSharedMemory->lock();
    buffer_temp_.setData(static_cast<const char *>(qSharedMemory->constData()), qSharedMemory->size());
    buffer_temp_.open(QBuffer::ReadOnly);
    /*
    QString memory;
    read>>memory;
    ipc_data_->ipc_mode_=(IPC_MODE)memory.section("/",1,1).toInt();
    ipc_data_->command_mode_=(COMMAND_MODE)memory.section("/",2,2).toInt();
    ipc_data_->data=memory.section("/",3,3);
    */
    QMap<QString,QString> memory_map_;
    read>>memory_map_;
    ipc_data_->ipc_mode_=(IPC_MODE)memory_map_["IPC_MODE"].toInt();
    ipc_data_->command_mode_=(COMMAND_MODE)memory_map_["COMMAND_MODE"].toInt();
    ipc_data_->data=memory_map_["DATA"];

    qSharedMemory->unlock();
    //qSharedMemory->detach();

    return true;
}
    //
    //
    //  Send/recive message using sharedMemory
    //  Take care of block in write/read method
    //
    //
inline bool ipc::check_command(QSharedMemory* qSharedMemory,IPC_DATA* ipc_check_data_,COMMAND_MODE commamd_mode_)
{
    QTime sr_time_;
    //std::cout<<"Check command: "<<std::endl;
    sr_time_.start();
    //while(sr_time_.elapsed()<ipc::SR_MESSAGE_TOLERANCE*1000)
    while(true)
    {
        if(ipc::read_memory(qSharedMemory,ipc_check_data_))
        {
            break;
        }
        //QThread::sleep(ipc::IPC_CHECK_DELAY);
        //std::cout<<".";
    }
    if(ipc_check_data_->command_mode_==commamd_mode_/*||(int)ipc_check_data_->command_mode_==0*/)
    {
        //std::cout<<"command mode-----[pass!]"<<std::endl;
        return true;
    }
    //std::cout<<"command mode-----[wrong!]"<<std::endl;
    return false;
}
    //
    // when the command_mode_ fits,write ipc_data to memory;
    //
inline bool ipc::send_message(QSharedMemory* qSharedMemory,IPC_DATA* ipc_data_,COMMAND_MODE commamd_mode_)
{
    IPC_DATA* ipc_check_data_=new IPC_DATA;
    QTime sr_time_;
    //std::cout<<"//Start to send message://"<<std::endl;
    sr_time_.restart();
    //while(sr_time_.elapsed()<ipc::SR_MESSAGE_TOLERANCE*1000)
    while(true)
    {
        if(ipc::check_command(qSharedMemory,ipc_check_data_,commamd_mode_))
        {
            //qSharedMemory->lock();
            break;
        }
        //std::cout<<".";
        //QThread::sleep(SR_MESSAGE_DELAY);
    }
    //std::cout<<std::endl;
    if(int timeS=sr_time_.restart()>ipc::SR_MESSAGE_TOLERANCE*1000)
    {
        std::cout<<"send overtime! use:"<<timeS<<std::endl;
        //return false;
    }
    //bool message_sent=
    while(true)
    {
        if(ipc::write_memory(qSharedMemory,ipc_data_))
        {
           break;
        }
    }
    //qSharedMemory->unlock();
    //qSharedMemory->connect(qSharedMemory,)
    return true;
}

    //
    //When the command_mode_ fits, read from memory to ipc_data_
    //
inline bool ipc::recive_message(QSharedMemory* qSharedMemory,IPC_DATA* ipc_data_,COMMAND_MODE commamd_mode_)
{
    IPC_DATA* ipc_check_data_=new IPC_DATA;
    QTime sr_time_;
    //std::cout<<"//Start to recive message://"<<std::endl;
    sr_time_.start();
    //while(sr_time_.elapsed()<ipc::SR_MESSAGE_TOLERANCE*1000)
    while(true)
    {
        if(ipc::check_command(qSharedMemory,ipc_check_data_,commamd_mode_))
        {
            //ipc_check_data_->print();
            //qSharedMemory->lock();
            break;
        }
        //std::cout<<".";
        //QThread::sleep(SR_MESSAGE_DELAY);
    }
    //std::cout<<std::endl;
    if(sr_time_.elapsed()>ipc::SR_MESSAGE_TOLERANCE*1000)
    {
        std::cout<<"recive overtime!";
        return false;
    }
    //ipc_data_=ipc_check_data_;
    ipc_data_->setValue(ipc_check_data_->ipc_mode_,ipc_check_data_->command_mode_,ipc_check_data_->data);
    //bool message_recived_=ipc::read_memory(qSharedMemory,ipc_data_);
    //ipc_data_->print();
    //qSharedMemory->unlock();
    return true;
}

























