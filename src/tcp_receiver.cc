#include "tcp_receiver.hh"

#include <algorithm>

using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg)
{
    const TCPHeader head = seg.header();
    string data = seg.payload().copy();
    cout << "data  :" << data << endl;

    uint64_t checkpoint = _reassembler.ack_index();
    cout << "CHECKPOINT :" << checkpoint << endl;
    uint64_t abs_seqno = unwrap(head.seqno, _isn, checkpoint);
    uint64_t stream_idx = abs_seqno - _synReceived;
    if (head.syn)
    {
        // ackidx = head.seqno + 1;
        _isn = head.seqno;
        _synReceived = true;
        _reassembler.push_substring(data, 0, head.fin);
    }

    if (!head.fin)
    {
        cout << "PUSH :" << data << " :stream_idx :" << stream_idx << endl;
        _reassembler.push_substring(data, stream_idx, false);
    }
    else
    {
        _finReceived = true;
        _reassembler.push_substring(data, stream_idx, true);
    }
        // ackidx = ackidx + data.size();
    // stream_ = wrap(_reassembler.stream_out().bytes_read() + _reassembler.stream_out().buffer_size() + 1, _isn);

    cout << "************" << endl;
}

optional<WrappingInt32> TCPReceiver::ackno() const
{
    if (_synReceived == false)
        return {};
    else
        if(_finReceived && _reassembler.unassembled_bytes()==0){
            return wrap(_reassembler.ack_index() + 2, _isn);
            
        }
            
        else 
            return wrap(_reassembler.ack_index() + 1, _isn);
}

size_t TCPReceiver::window_size() const
{
    return _reassembler.stream_out().remaining_capacity();
}
