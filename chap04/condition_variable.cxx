std::mutex mut;
std::queue<data_chunk> data_queue;    ⇽---  ①
std::condition_variable data_cond;
void data_preparation_thread()            // 由线程乙运行
{
    while(more_data_to_prepare())
    {
        data_chunk const data=prepare_data();
        {
            std::lock_guard<std::mutex> lk(mut);
            data_queue.push(data);    ⇽---  ②
        }
        data_cond.notify_one();    ⇽---  ③
    }
}
void data_processing_thread()           // 由线程甲运行
{
    while(true)
    {
        std::unique_lock<std::mutex> lk(mut);    ⇽---  ④
        data_cond.wait(
            lk,[]{return !data_queue.empty();});    ⇽---  ⑤
        data_chunk data=data_queue.front();
        data_queue.pop();
        lk.unlock();    ⇽---  ⑥
        process(data);
        if(is_last_chunk(data))
            break;
    }
}