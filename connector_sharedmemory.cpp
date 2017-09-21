#include "connector_sharedmemory.h"





ipc::ipc_sharedmemory::ipc_sharedmemory(IPC_MODE connector_mode,const std::string connector_name)
{
    this->connector_name=connector_name;
    this->connector_mode=connector_mode;
    std::cout<<"connector_mode is:"<<(int)this->connector_mode<<std::endl;
    std::cout<<"connector_name is:"<<this->connector_name<<std::endl;
}

/*
ipc::ipc_sharedmemory::ipc_sharedmemory(const ipc_sharedmemory & ipc)
{
    connector_name=ipc.get_connector_name();
    connector_mode=ipc.get_connector_mode();
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
    if(this->connector_mode==ipc_mode)
    {
        std::stringstream ss;
        ss<<"Wrong connector mode";
        std::cout<<ss.str();
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
        //ipc_memory_=  new QSharedMemory(connector_name);
        ipc_memory_->setKey(QString::fromStdString(connector_name));
        break;

        case IPC_MODE::MODE_RESPOND:
        //ipc_memory_=  new QSharedMemory(connector_name);
        //IPC_DATA*defaule_data_= new IPC_DATA;
        ipc_memory_->setKey(QString::fromStdString(connector_name));
        //ipc::write_memory(ipc_memory_,defaule_data_);

        break;
    }
}

std::string ipc::ipc_sharedmemory::req(const std::string& request)
{
    check_mode(IPC_MODE::MODE_REQUEST);
    IPC_DATA* req_data_=new IPC_DATA;
    req_data_->setValue(IPC_MODE::MODE_REQUEST,COMMAND_MODE::COMMAND_REQUEST,QString::fromStdString(request));

    while(true)
    {
        if(ipc::send_message(ipc_memory_,req_data_,COMMAND_MODE::COMMAND_DEFAULT))
        {
               break;

        }
        else if(ipc::send_message(ipc_memory_,req_data_,COMMAND_MODE::COMMAND_RECIVE))
        {
               break;
        }
        QThread::sleep(SR_MESSAGE_DELAY);
    }
    while(true)
    {
        req_data_->command_mode_=COMMAND_MODE::COMMAND_RECIVE;
        if(ipc::recive_message(ipc_memory_,req_data_,COMMAND_MODE::COMMAND_RESPONSE))
        {
               break;
        }
        QThread::sleep(SR_MESSAGE_DELAY);
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
        QThread::sleep(SR_MESSAGE_DELAY);
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
        QThread::sleep(SR_MESSAGE_DELAY);
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
bool ipc::write_memory(QSharedMemory* qSharedMemory,IPC_DATA* ipc_data_)
{
    if(!qSharedMemory->isAttached())
    {
        qSharedMemory->attach();
    }

    //==================write to buffer=================//
    QBuffer buffer_temp_;
    buffer_temp_.open(QBuffer::ReadWrite);
    QDataStream write(&buffer_temp_);
    QString memory=QString("/%1/%2/%3/").
            arg((int)ipc_data_->ipc_mode_).
            arg((int)ipc_data_->command_mode_).
            arg(ipc_data_->data)
            ;
    write<<memory;
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

bool ipc::read_memory(QSharedMemory* qSharedMemory,IPC_DATA* ipc_data_)
{

    QTime sr_time_;
    std::cout<<"Read memory..."<<std::endl;
    sr_time_.start();
    /*
    while(sr_time_.elapsed()<ipc::WR_MEMORY_TOLERANCE*1000)
    {
        if(qSharedMemory->attach())
        {
            break;
        }
        QThread::sleep(ipc::WR_MEMORY_DELAY);
    }
    if(sr_time_.elapsed()>ipc::WR_MEMORY_TOLERANCE*1000)
    {
        std::cout<<"read overtime!"<<std::endl;
        return false;

    }
    */
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
    QString memory;
    read>>memory;

    ipc_data_->ipc_mode_=(IPC_MODE)memory.section("/",1,1).toInt();
    ipc_data_->command_mode_=(COMMAND_MODE)memory.section("/",2,2).toInt();
    ipc_data_->data=memory.section("/",3,3);
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
bool ipc::check_command(QSharedMemory* qSharedMemory,IPC_DATA* ipc_check_data_,COMMAND_MODE commamd_mode_)
{
    QTime sr_time_;
    std::cout<<"Check command: "<<std::endl;
    sr_time_.start();
    //while(sr_time_.elapsed()<ipc::SR_MESSAGE_TOLERANCE*1000)
    while(true)
    {
        if(ipc::read_memory(qSharedMemory,ipc_check_data_))
        {
            break;
        }
        QThread::sleep(ipc::IPC_CHECK_DELAY);
        std::cout<<".";
    }
    if(ipc_check_data_->command_mode_==commamd_mode_/*||(int)ipc_check_data_->command_mode_==0*/)
    {
        std::cout<<"command mode-----[pass!]"<<std::endl;
        return true;
    }
    std::cout<<"command mode-----[wrong!]"<<std::endl;
    return false;
}
    //
    // when the command_mode_ fits,write ipc_data to memory;
    //
bool ipc::send_message(QSharedMemory* qSharedMemory,IPC_DATA* ipc_data_,COMMAND_MODE commamd_mode_)
{
    IPC_DATA* ipc_check_data_=new IPC_DATA;
    QTime sr_time_;
    std::cout<<"//================================Start to send message:====================================//"<<std::endl;
    sr_time_.restart();
    //while(sr_time_.elapsed()<ipc::SR_MESSAGE_TOLERANCE*1000)
    while(true)
    {
        if(ipc::check_command(qSharedMemory,ipc_check_data_,commamd_mode_))
        {
            qSharedMemory->lock();
            break;
        }
        std::cout<<".";
        QThread::sleep(SR_MESSAGE_DELAY);
    }
    //std::cout<<std::endl;
    if(int timeS=sr_time_.restart()>ipc::SR_MESSAGE_TOLERANCE*1000)
    {
        std::cout<<"send overtime! use:"<<timeS<<std::endl;
        return false;
    }
    bool message_sent=ipc::write_memory(qSharedMemory,ipc_data_);

    return message_sent;
}

    //
    //When the command_mode_ fits, read from memory to ipc_data_
    //
bool ipc::recive_message(QSharedMemory* qSharedMemory,IPC_DATA* ipc_data_,COMMAND_MODE commamd_mode_)
{
    IPC_DATA* ipc_check_data_=new IPC_DATA;
    QTime sr_time_;
    std::cout<<"//================================Start to recive message:==================================//"<<std::endl;
    sr_time_.start();
    //while(sr_time_.elapsed()<ipc::SR_MESSAGE_TOLERANCE*1000)
    while(true)
    {
        if(ipc::check_command(qSharedMemory,ipc_check_data_,commamd_mode_))
        {
            qSharedMemory->lock();
            break;
        }
        std::cout<<".";
        QThread::sleep(SR_MESSAGE_DELAY);
    }
    //std::cout<<std::endl;
    if(sr_time_.elapsed()>ipc::SR_MESSAGE_TOLERANCE*1000)
    {
        std::cout<<"recive overtime!";
        return false;
    }
    bool message_recived_=ipc::read_memory(qSharedMemory,ipc_data_);
    ipc_data_->print();

    return message_recived_;
}












