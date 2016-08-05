#pragma once

// from: http://stackoverflow.com/a/17508042/1608780
#if defined _WIN32 || defined WIN32 || defined OS_WIN64 || defined _WIN64 || defined WIN64 || defined WINNT
class send_timeout
{
public:
	send_timeout(unsigned int timeout_millis) : timeout_millis_(timeout_millis) {};

	template<class Protocol>
	int level(const Protocol& p) const { return SOL_SOCKET; }

	template<class Protocol>
	int name(const Protocol& p) const { return SO_SNDTIMEO; }

	template<class Protocol>
	const void* data(const Protocol& p) const { return &timeout_millis_; }

	template<class Protocol>
	size_t size(const Protocol& p) const { return sizeof(timeout_millis_); }
private:
	unsigned int timeout_millis_;
};

class recv_timeout
{
public:
	recv_timeout(unsigned int timeout_millis) : timeout_millis_(timeout_millis) {};

	template<class Protocol>
	int level(const Protocol& p) const { return SOL_SOCKET; }

	template<class Protocol>
	int name(const Protocol& p) const { return SO_RCVTIMEO; }

	template<class Protocol>
	const void* data(const Protocol& p) const { return &timeout_millis_; }

	template<class Protocol>
	size_t size(const Protocol& p) const { return sizeof(timeout_millis_); }
private:
	unsigned int timeout_millis_;
};

#else
class send_timeout
{
public:
	send_timeout(unsigned int timeout_millis) {
		struct timeval tv;
		tv.tv_sec = timeout_millis / 1000;
		tv.tv_usec = timeout_millis % 1000;
		timeout_millis_ = tv;
	};

	template<class Protocol>
	int level(const Protocol& p) const { return SOL_SOCKET; }

	template<class Protocol>
	int name(const Protocol& p) const { return SO_SNDTIMEO; }

	template<class Protocol>
	const void* data(const Protocol& p) const { return &timeout_millis_; }

	template<class Protocol>
	size_t size(const Protocol& p) const { return sizeof(timeout_millis_); }
private:
	timeval timeout_millis_;
};

class recv_timeout
{
public:
	recv_timeout(unsigned int timeout_millis) {
		struct timeval tv;
		tv.tv_sec = timeout_millis / 1000;
		tv.tv_usec = timeout_millis % 1000;
		timeout_millis_ = tv;
	};

	template<class Protocol>
	int level(const Protocol& p) const { return SOL_SOCKET; }

	template<class Protocol>
	int name(const Protocol& p) const { return SO_RCVTIMEO; }

	template<class Protocol>
	const void* data(const Protocol& p) const { return &timeout_millis_; }

	template<class Protocol>
	size_t size(const Protocol& p) const { return sizeof(timeout_millis_); }
private:
	timeval timeout_millis_;
};
#endif