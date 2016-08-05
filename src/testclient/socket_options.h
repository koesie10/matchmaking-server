#pragma once

// from: http://stackoverflow.com/a/17508042/1608780
class send_timeout
{
public:
	send_timeout(unsigned int dwTimeout) : m_dwTimeout(dwTimeout) {};

	template<class Protocol>
	int level(const Protocol& p) const { return SOL_SOCKET; }

	template<class Protocol>
	int name(const Protocol& p) const { return SO_SNDTIMEO; }

	template<class Protocol>
	const void* data(const Protocol& p) const { return &m_dwTimeout; }

	template<class Protocol>
	size_t size(const Protocol& p) const { return sizeof(m_dwTimeout); }
private:
	unsigned int m_dwTimeout;
};

class recv_timeout
{
public:
	recv_timeout(unsigned int dwTimeout) : m_dwTimeout(dwTimeout) {};

	template<class Protocol>
	int level(const Protocol& p) const { return SOL_SOCKET; }

	template<class Protocol>
	int name(const Protocol& p) const { return SO_RCVTIMEO; }

	template<class Protocol>
	const void* data(const Protocol& p) const { return &m_dwTimeout; }

	template<class Protocol>
	size_t size(const Protocol& p) const { return sizeof(m_dwTimeout); }
private:
	unsigned int m_dwTimeout;
};