#include "stream_reassembler.hh"

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&.../* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity)
    : _output(capacity), buffer(), startidx(0), capa(capacity),eoff(false),eofidx(-1)
{
}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof)
{
    if (index >= _output.bytes_read() + capa)
    {
        return;
    }

    if(eof){
        eofidx=index+data.size();
    }

    size_t str_siz = data.size();
    size_t endidx = index + str_siz;
    if (endidx > _output.bytes_read() + capa)
    {
        size_t idx = max(index, startidx);
        string tmp = data.substr(idx-index, _output.bytes_read() + capa - index);
        buffer[idx] = tmp;
    }
    else
    {
        if(data=="" && eof) _output.end_input();
        if(endidx<=startidx) return;
        size_t idx = max(index, startidx);
        string tmp = data.substr(idx-index, data.size()-(idx-index));
        buffer[idx] = tmp;
        if(eof && idx==startidx)
        {
            eoff = true;
        }
    }

    for (auto it : buffer)
    {
        if (it.first <= startidx && _output.remaining_capacity())
        {
            if(it.first+it.second.size()<=startidx) continue;
            size_t idx = max(it.first, startidx);
            string tmp = it.second.substr(idx-it.first, it.second.size()-(idx-it.first));
            _output.write(tmp);
            startidx = tmp.size() + startidx;
        }
        else
        {
            break;
        }
    }

    while (buffer.size() && buffer.begin()->first < startidx)
    {
        buffer.erase(buffer.begin());
    }

    if (eoff || startidx==eofidx)
    {
        _output.end_input();
    }
}

size_t StreamReassembler::unassembled_bytes() const
{
    unordered_map<char, size_t> freq;
    for (auto it : buffer)
    {
        for (auto c : it.second)
        {
            freq[c]++;
        }
    }
    return freq.size();
}

bool StreamReassembler::empty() const { return buffer.size() == 0; }

size_t StreamReassembler::ack_index() const { return startidx; }
