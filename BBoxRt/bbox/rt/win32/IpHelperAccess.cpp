/**
* @file
*
* Implementation for the bbox::rt::win32::IpHelperAccess class.
*/

#ifdef WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <IPHlpApi.h>
#include <Mstcpip.h>
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "Ntdll.lib")
#endif

#include <bbox/rt/win32/IpHelperAccess.h>

#include <bbox/Assert.h>
#include <bbox/TextCoding.h>
#include <bbox/Format.h>

#ifdef WIN32

namespace {

    struct ChangeDetails
    {
        void(*func)(void *context);
        void *context;
        void *handle;
    };

    VOID __stdcall InterfaceChangeCallback(PVOID caller_context, PMIB_IPINTERFACE_ROW interface_row, MIB_NOTIFICATION_TYPE notification_type)
    {
        ChangeDetails *details = static_cast<ChangeDetails *>(caller_context);

        details->func(details->context);
    }

} // annonymous namespace

namespace bbox {
    namespace rt {
        namespace win32 {

            bbox::Error  IpHelperAccess::RegisterChangeNotifier(void *&notifier, void(*handler)(void *context), void *context)
            {
                bbox::Error result;

                if (notifier == nullptr)
                {
                    ChangeDetails *details = new ChangeDetails;

                    details->func = handler;
                    details->context = context;

                    DWORD win32_result = ::NotifyIpInterfaceChange(
                        AF_UNSPEC,
                        &InterfaceChangeCallback,
                        details,
                        TRUE,
                        &details->handle);

                    if (win32_result == NO_ERROR)
                    {
                        notifier = details;
                    }
                    else
                    {
                        result = bbox::Error::Win32_GetLastError();

                        delete details;
                    }
                }
                else
                {
                    result = std::errc::already_connected;
                }

                return result;
            }

            bbox::Error IpHelperAccess::DestroyChangeNotifier(void *&notifier)
            {
                if (notifier != nullptr)
                {
                    ChangeDetails *details = static_cast<ChangeDetails *>(notifier);

                    ::CancelMibChangeNotify2(details->handle);

                    delete details;

                    notifier = nullptr;
                }

                return bbox::Error();
            }

            bbox::Error IpHelperAccess::QueryAdapters(std::map<std::string, net::AdapterInfo> &results)
            {
                bbox::Error error;

                results.clear();

                ULONG buffer_length = 0;
                void *buffer = nullptr;

                while (true)
                {
                    PIP_ADAPTER_ADDRESSES result_info = static_cast<PIP_ADAPTER_ADDRESSES>(buffer);
                    ULONG result_length = buffer_length;

                    DWORD win32_result = ::GetAdaptersAddresses(
                        AF_UNSPEC,
                        0,
                        nullptr,
                        result_info,
                        &result_length);

                    if (win32_result == ERROR_BUFFER_OVERFLOW)
                    {
                        // We need a larger buffer size - update
                        // the buffer size

                        BBOX_ASSERT(result_length > buffer_length);

                        buffer = realloc(buffer, result_length);
                        buffer_length = result_length;
                    }
                    else if (win32_result == ERROR_NO_DATA)
                    {
                        // There are simply no adapters - just
                        // return an empty list

                        break;
                    }
                    else if (win32_result != NO_ERROR)
                    {
                        // Some other error - return the error code

                        error = bbox::Error::Win32_GetLastError();
                        break;
                    }
                    else
                    {
                        // Success - convert the info from the Windows
                        // structures into our internal format

                        for (PIP_ADAPTER_ADDRESSES adapter = result_info;
                             adapter != nullptr;
                             adapter = adapter->Next)
                        {
                            if ((adapter->OperStatus == IfOperStatusUp)
                                && ((adapter->Flags & (IP_ADAPTER_RECEIVE_ONLY | IP_ADAPTER_NO_MULTICAST)) == 0)
                                && ((adapter->Flags & (IP_ADAPTER_IPV4_ENABLED | IP_ADAPTER_IPV6_ENABLED)) != 0))
                            {
                                std::string system_name, mac, friendly_name, description;

                                system_name = adapter->AdapterName;

                                for (size_t i = 0; i < adapter->PhysicalAddressLength; ++i)
                                {
                                    if (!mac.empty())
                                        mac.push_back('-');
                                    mac.append(Format("%02X", adapter->PhysicalAddress[i]));
                                }

                                error = TextCoding::Win32_UTF16_to_UTF8(std::wstring(adapter->FriendlyName), friendly_name);
                                if (error)
                                    break;

                                error = TextCoding::Win32_UTF16_to_UTF8(std::wstring(adapter->Description), description);
                                if (error)
                                    break;

                                net::AdapterInfo &info = results[system_name];
                                info.system_name = std::move(system_name);
                                info.user_name = std::move(friendly_name);
                                info.description = std::move(description);
                                info.mac_address = std::move(mac);

                                for (PIP_ADAPTER_UNICAST_ADDRESS address = adapter->FirstUnicastAddress;
                                     address != nullptr;
                                     address = address->Next)
                                {
                                    if ((address->Flags & IP_ADAPTER_ADDRESS_TRANSIENT) == 0)
                                    {
                                        if (address->Address.lpSockaddr->sa_family == AF_INET)
                                        {
                                            BBOX_ASSERT(address->Address.iSockaddrLength >= sizeof(sockaddr_in));
                                            sockaddr_in *addr = reinterpret_cast<sockaddr_in *>(address->Address.lpSockaddr);

                                            WCHAR str_buf[INET_ADDRSTRLEN];
                                            ULONG str_len = INET_ADDRSTRLEN;

                                            win32_result = RtlIpv4AddressToStringEx(
                                                &addr->sin_addr,
                                                0,
                                                str_buf,
                                                &str_len);
                                            if (win32_result != NO_ERROR)
                                            {
                                                error = Error::Win32_GetLastError();
                                                break;
                                            }

                                            std::string str;

                                            error = TextCoding::Win32_UTF16_to_UTF8(std::wstring(str_buf, str_len), str);
                                            if (error)
                                                break;

                                            info.ip_addresses.insert(str);
                                        }
                                        else if (address->Address.lpSockaddr->sa_family == AF_INET6)
                                        {
                                            BBOX_ASSERT(address->Address.iSockaddrLength >= sizeof(sockaddr_in6));
                                            sockaddr_in6 *addr = reinterpret_cast<sockaddr_in6 *>(address->Address.lpSockaddr);

                                            WCHAR str_buf[INET6_ADDRSTRLEN];
                                            ULONG str_len = INET6_ADDRSTRLEN;

                                            win32_result = RtlIpv6AddressToStringEx(
                                                &addr->sin6_addr,
                                                addr->sin6_scope_id,
                                                0,
                                                str_buf,
                                                &str_len);
                                            if (win32_result != NO_ERROR)
                                            {
                                                error = Error::Win32_GetLastError();
                                                break;
                                            }

                                            std::string str;

                                            error = TextCoding::Win32_UTF16_to_UTF8(std::wstring(str_buf, str_len), str);
                                            if (error)
                                                break;

                                            info.ip_addresses.insert(str);
                                        }
                                    }
                                }
                                if (error)
                                    break;
                            }
                        }

                        break;
                    }
                }

                // Finally - ensure the buffer is freed up
                // and that we clear the results if any error
                // occured

                free(buffer);

                if (error)
                    results.clear();

                return error;
            }

        } // namespace bbox::rt::win32
    } // namespace bbox::rt
} // namespace bbox

#else // not WIN32

namespace bbox {
    namespace rt {
        namespace win32 {

            bbox::Error  IpHelperAccess::RegisterChangeNotifier(void *&notifier, void(*handler)(void *context), void *context)
            {
                // TODO
                std::cerr << "TODO: Network change notification not implemented yet!" << std::endl;
                return bbox::Error();
            }

            bbox::Error IpHelperAccess::DestroyChangeNotifier(void *&notifier)
            {
                // TODO
                return bbox::Error();
            }

            bbox::Error IpHelperAccess::QueryAdapters(std::map<std::string, net::AdapterInfo> &results)
            {
                return bbox::Error();
            }

        } // namespace bbox::rt::win32
    } // namespace bbox::rt
} // namespace bbox

#endif // not WIN32