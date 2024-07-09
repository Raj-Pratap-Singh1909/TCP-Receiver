#include "byte_stream.hh"

#include <algorithm>

// You will need to add private members to the class declaration in `byte_stream.hh`

/* Replace all the dummy definitions inside the methods in this file. */


using namespace std;

ByteStream::ByteStream(const size_t capa): eof_reached(false),siz(capa),length(capa),written(0),popped(0),buf()
{}

size_t ByteStream::write(const string &data) {
	size_t str_siz=data.size();
	size_t end=min(str_siz,length);
	for(size_t i=0;i<end;i++){
		buf.push_back(data[i]);
}
	length=length-end;
	written+=end;
  return end;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
	string ret;
	size_t pr=siz-length;
	pr=min(pr,len);
	for(size_t i=0;i<pr;i++){
		ret+=buf[i];
}
  return ret;
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
	size_t buf_size=siz-length;
	if(len>buf_size) set_error();
	else{
		for(size_t i=0;i<len;i++){
			buf.pop_front();
			popped++;
		}
		length+=len;
	}

}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
	string ret;
	size_t buf_size=siz-length;
	if(len>buf_size) set_error();
	else{
		for(size_t i=0;i<len;i++){
			ret+=buf.front();
			buf.pop_front();
			popped++;
		}
		length+=len;
	}	

  return ret;
}

void ByteStream::end_input() {
	eof_reached=true;
 }

bool ByteStream::input_ended() const { return eof_reached;}

size_t ByteStream::buffer_size() const {return siz-length; }

bool ByteStream::buffer_empty() const {
	
	if(buffer_size()==0)	return true;
	return false; 
}

bool ByteStream::eof() const { 
	if(buffer_empty() && input_ended()) return true;
	return false; 
}

size_t ByteStream::bytes_written() const { return written; }

size_t ByteStream::bytes_read() const { return popped;  }

size_t ByteStream::remaining_capacity() const { return length; }
