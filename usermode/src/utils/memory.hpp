#pragma once

#include "cordrv.hpp"

#define NT_SUCCESS(status) (static_cast<long>(status) >= 0)

struct __system_handle_t
{
	unsigned long m_process_id;
	uint8_t m_object_type_number;
	uint8_t m_flags;
	uint16_t m_handle;
	void* m_object;
	ACCESS_MASK m_granted_access;
};

struct system_handle_info_t
{
	unsigned long m_handle_count;
	__system_handle_t m_handles[1];
};

class c_memory
{
public:
	~c_memory()
	{
		
			
		m_drv.Close();
	}

	bool setup();
	std::optional<uint32_t> get_process_id(const std::string_view& process_name);
	
	std::optional<c_address> find_pattern(const std::string_view& module_name, const std::string_view& pattern);
	std::pair<std::optional<uintptr_t>, std::optional<uintptr_t>> get_module_info(const std::string_view& module_name);
	bool is_anticheat_running();

	bool read_t(const uintptr_t address, void* buffer, uintptr_t size)
	{
		this->read_memory(reinterpret_cast<void*>(address), buffer, size);
		return true;
	}

	template <typename t>
	t read_t(void* address)
	{
		t value{ 0 };
		this->read_memory(address, &value, sizeof(t));
		return value;
	}

	template <typename T>
	T read_t(const uintptr_t address) noexcept
	{
		T buffer{};
		this->read_memory(reinterpret_cast<void*>(address), &buffer, sizeof(T));
		return buffer;
	}

	template <>
	std::string read_t<std::string>(const uintptr_t address)
	{
		static const int length = 64;
		std::vector<char> buffer(length);

		this->read_memory(reinterpret_cast<void*>(address), buffer.data(), length);

		const auto& it = find(buffer.begin(), buffer.end(), '\0');

		if (it != buffer.end())
			buffer.resize(distance(buffer.begin(), it));

		return std::string(buffer.begin(), buffer.end());
	}

private:
	bool m_initialized = false;
	
	uint32_t m_id = 0;
	
	CorDrv m_drv;
	uint64_t m_process_dtb = 0;

	bool read_memory(void* address, void* buffer, const size_t size)
	{
		return m_drv.ReadProcessMemory(m_process_dtb, reinterpret_cast<uint64_t>(address), buffer, size);
	}
};

inline const std::unique_ptr<c_memory> m_memory{ new c_memory() };
