/*
 * If not stated otherwise in this file or this component's LICENSE file the
 * following copyright and licenses apply:
 *
 * Copyright 2020 Metrological
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __COM_PROCESSLAUNCH_H
#define __COM_PROCESSLAUNCH_H

#include <atomic>
#include "Module.h"


#include "Administrator.h"
#include "ICOM.h"

#include "IUnknown.h"
#include "Ids.h"


#ifdef PROCESSCONTAINERS_ENABLED
#include <processcontainers/ProcessContainer.h>
#endif


#if defined(WARNING_REPORTING_ENABLED)
#include "../warningreporting/WarningReportingUnit.h"
#endif

namespace WPEFramework {
namespace RPC {

    class EXTERNAL Object {
    public:
        enum class HostType {
            LOCAL,
            DISTRIBUTED,
            CONTAINER
        };

        Object()
            : _locator()
            , _className()
            , _callsign()
            , _interface(~0)
            , _version(~0)
            , _user()
            , _group()
            , _threads()
            , _priority()
            , _type(HostType::LOCAL)
            , _systemRootPath()
            , _remoteAddress()
            , _configuration()
        {
        }
        Object(const Object& copy)
            : _locator(copy._locator)
            , _className(copy._className)
            , _callsign(copy._callsign)
            , _interface(copy._interface)
            , _version(copy._version)
            , _user(copy._user)
            , _group(copy._group)
            , _threads(copy._threads)
            , _priority(copy._priority)
            , _type(copy._type)
            , _systemRootPath(copy._systemRootPath)
            , _remoteAddress(copy._remoteAddress)
            , _configuration(copy._configuration)
        {
        }
        Object(const string& locator,
            const string& className,
            const string& callsign,
            const uint32_t interface,
            const uint32_t version,
            const string& user,
            const string& group,
            const uint8_t threads,
            const int8_t priority,
            const HostType type,
            const string& systemRootPath,
            const string& remoteAddress,
            const string& configuration)
            : _locator(locator)
            , _className(className)
            , _callsign(callsign)
            , _interface(interface)
            , _version(version)
            , _user(user)
            , _group(group)
            , _threads(threads)
            , _priority(priority)
            , _type(type)
            , _systemRootPath(systemRootPath)
            , _remoteAddress(remoteAddress)
            , _configuration(configuration)
        {
        }
        ~Object()
        {
        }

        Object& operator=(const Object& RHS)
        {
            _locator = RHS._locator;
            _className = RHS._className;
            _callsign = RHS._callsign;
            _interface = RHS._interface;
            _version = RHS._version;
            _user = RHS._user;
            _group = RHS._group;
            _threads = RHS._threads;
            _priority = RHS._priority;
            _systemRootPath = RHS._systemRootPath;
            _type = RHS._type;
            _remoteAddress = RHS._remoteAddress;
            _configuration = RHS._configuration;

            return (*this);
        }

    public:
        inline const string& Locator() const
        {
            return (_locator);
        }
        inline const string& ClassName() const
        {
            return (_className);
        }
        inline const string& Callsign() const
        {
            return (_callsign);
        }
        inline uint32_t Interface() const
        {
            return (_interface);
        }
        inline uint32_t Version() const
        {
            return (_version);
        }
        inline const string& User() const
        {
            return (_user);
        }
        inline const string& Group() const
        {
            return (_group);
        }
        inline uint8_t Threads() const
        {
            return (_threads);
        }
        inline int8_t Priority() const
        {
            return (_priority);
        }
        inline HostType Type() const
        {
            return (_type);
        }
        inline const string& SystemRootPath() const
        {
            return (_systemRootPath);
        }
        inline const Core::NodeId RemoteAddress() const
        {
            return (Core::NodeId(_remoteAddress.c_str()));
        }
        inline const string& Configuration() const
        {
            return (_configuration);
        }

    private:
        string _locator;
        string _className;
        string _callsign;
        uint32_t _interface;
        uint32_t _version;
        string _user;
        string _group;
        uint8_t _threads;
        int8_t _priority;
        HostType _type;
        string _systemRootPath;
        string _remoteAddress;
        string _configuration;
    };

    class EXTERNAL Config {
    private:
        Config& operator=(const Config&);

    public:
        Config()
            : _connector()
            , _hostApplication()
            , _persistent()
            , _system()
            , _data()
            , _volatile()
            , _application()
            , _proxyStub()
            , _postMortem()
        {
        }
        Config(
            const string& connector,
            const string& hostApplication,
            const string& persistentPath,
            const string& systemPath,
            const string& dataPath,
            const string& volatilePath,
            const string& applicationPath,
            const string& proxyStubPath,
            const string& postMortem)
            : _connector(connector)
            , _hostApplication(hostApplication)
            , _persistent(persistentPath)
            , _system(systemPath)
            , _data(dataPath)
            , _volatile(volatilePath)
            , _application(applicationPath)
            , _proxyStub(proxyStubPath)
            , _postMortem(postMortem)
        {
        }
        Config(const Config& copy)
            : _connector(copy._connector)
            , _hostApplication(copy._hostApplication)
            , _persistent(copy._persistent)
            , _system(copy._system)
            , _data(copy._data)
            , _volatile(copy._volatile)
            , _application(copy._application)
            , _proxyStub(copy._proxyStub)
            , _postMortem(copy._postMortem)
        {
        }
        ~Config()
        {
        }

    public:
        inline const string& Connector() const
        {
            return (_connector);
        }
        inline const string& HostApplication() const
        {
            return (_hostApplication);
        }
        inline const string& PersistentPath() const
        {
            return (_persistent);
        }
        inline const string& SystemPath() const
        {
            return (_system);
        }
        inline const string& DataPath() const
        {
            return (_data);
        }
        inline const string& VolatilePath() const
        {
            return (_volatile);
        }
        inline const string& ApplicationPath() const
        {
            return (_application);
        }
        inline const string& ProxyStubPath() const
        {
            return (_proxyStub);
        }
        inline const string& PostMortemPath() const
        {
            return (_postMortem);
        }

    private:
        string _connector;
        string _hostApplication;
        string _persistent;
        string _system;
        string _data;
        string _volatile;
        string _application;
        string _proxyStub;
        string _postMortem;
    };

    struct EXTERNAL IMonitorableProcess : public virtual Core::IUnknown {
        enum { ID = ID_MONITORABLE_PROCESS };

        ~IMonitorableProcess() override = default;

        virtual string Callsign() const = 0;
        virtual Core::instance_id ParentPID() const = 0;
    };

    class EXTERNAL InvokeHandler : public Core::IIPCServer {
    public:
        InvokeHandler(InvokeHandler&&) = delete;
        InvokeHandler(const InvokeHandler&) = delete;
        InvokeHandler& operator=(InvokeHandler&&) = delete;
        InvokeHandler& operator=(const InvokeHandler&) = delete;

        InvokeHandler() = default;
        ~InvokeHandler() override = default;

    public:
        void Procedure(Core::IPCChannel& channel, Core::ProxyType<Core::IIPC>& data) override
        {
            Core::ProxyType<Core::IPCChannel> proxyChannel(channel);
            Job::Invoke(proxyChannel, data);
        }
    };

    class EXTERNAL Communicator {
    private:
        friend class ProcessShutdown;
        class RemoteConnectionMap;

    public:
        class ChannelLink;
        using Client = Core::IPCChannelServerType<ChannelLink, true>::Client;

        class EXTERNAL RemoteConnection : public IRemoteConnection {
        private:
            friend class RemoteConnectionMap;

            RemoteConnection(RemoteConnection&&) = delete;
            RemoteConnection(const RemoteConnection&) = delete;
            RemoteConnection& operator=(RemoteConnection&&) = delete;
            RemoteConnection& operator=(const RemoteConnection&) = delete;

        protected:
            RemoteConnection()
                : _channel()
                , _id(_sequenceId++)
                , _remoteId(0) {
                    printf("WPEFramework::RPC::Communicator::RemoteConnection::RemoteConnection()->PID<%d><%dd>Constructor _channel, _id(_sequenceId++<>), _remoteId<%d>\n", getpid(), gettid(),_remoteId);
            }
            RemoteConnection(Core::ProxyType<Client>& channel, const uint32_t remoteId)
                : _channel(channel)
                , _id(_sequenceId++)
                , _remoteId(remoteId) {
                printf("WPEFramework::RPC::Communicator::RemoteConnection::RemoteConnection(channel, remoteId)->PID<%d><%dd>Constructor _channel, _id(_sequenceId++<>), _remoteId<%d>\n", getpid(), gettid(),_remoteId);
            }

        public:
            ~RemoteConnection() override = default;

        public:
            uint32_t Id() const override;
            uint32_t RemoteId() const override;
            void* Acquire(const uint32_t waitTime, const string& className, const uint32_t interfaceId, const uint32_t version) override;
            void Terminate() override;
            void PostMortem() override {
                // This is really something that needs to be done by the specific implementations.
            }
            uint32_t Launch() override {
                return (Core::ERROR_UNAVAILABLE);
            }

            inline bool IsOperational() const
            {
                return (_channel.IsValid() == true);
            }
            inline Core::ProxyType<Core::IPCChannel> Channel()
            {
                ASSERT(_channel.IsValid() == true);

                return (Core::ProxyType<Core::IPCChannel>(_channel));
            }
            void Open(Core::ProxyType<Client>& channel, const uint32_t id)
            {
                ASSERT(_channel.IsValid() == false);
                printf("WPEFramework::RPC::Communicator::RemoteConnection::Open()->PID<%d><%d> channel.IsValid()==false\n", getpid(), gettid());
                // Seems we received an interface from the otherside. Prepare the actual stub around it.
                TRACE_L1("Link announced. All up and running %d, has announced itself.", Id());

                printf("WPEFramework::RPC::Communicator::RemoteConnection::Open()->PID<%d><%d> _channel=channel, _remoteId = id\n", getpid(), gettid());
                _channel = channel;
                _remoteId = id;
            }
            void Close()
            {
                printf("WPEFramework::RPC::Communicator::RemoteConnection::Close()->PID<%d><%d> channel.IsValid()\n", getpid(), gettid());
                if (_channel.IsValid() == true) {
                    printf("WPEFramework::RPC::Communicator::RemoteConnection::Close()->PID<%d><%d> _channel->Source().Close(0)\n", getpid(), gettid());
                    _channel->Source().Close(0);
                }
            }

            BEGIN_INTERFACE_MAP(RemoteConnection)
                INTERFACE_ENTRY(IRemoteConnection)
            END_INTERFACE_MAP

        private:
            Core::ProxyType<Core::IPCChannelType<Core::SocketPort, ChannelLink>> _channel;
            uint32_t _id;
            uint32_t _remoteId;
            static std::atomic<uint32_t> _sequenceId;
        };
        class EXTERNAL MonitorableProcess : public RemoteConnection, public IMonitorableProcess {
        public:
            MonitorableProcess() = delete;
            MonitorableProcess(MonitorableProcess&&) = delete;
            MonitorableProcess(const MonitorableProcess&) = delete;
            MonitorableProcess& operator= (MonitorableProcess&&) = delete;
            MonitorableProcess& operator= (const MonitorableProcess&) = delete;

            MonitorableProcess(const string& callsign, RemoteConnectionMap& parent)
                : RemoteConnection()
                , IMonitorableProcess()
                , _parent(parent)
                , _callsign(callsign)
                , _cycle(0)
                , _time(0) {
                    printf("WPEFramework::RPC::Communicator::MonitorableProcess::MonitorableProcess()->PID<%d><%d>\n", getpid(), gettid());
            }
            ~MonitorableProcess() override = default;

        public:
            inline bool IsTerminated() const {
                return (_cycle == static_cast<uint8_t>(~0));
            }
            inline bool operator== (const RemoteConnectionMap& parent) const {
                return (&parent == &_parent);
            }
            inline bool operator!= (const RemoteConnectionMap& parent) const {
                return (&parent != &_parent);
            }
            inline string Callsign() const override {
                return (_callsign);
            }
            inline uint8_t Cycle() const {
                return (_cycle);
            }
            inline const uint64_t& Time() const {
                return (_time);
            }
            inline void Destruct() {
                printf("WPEFramework::RPC::Communicator::MonitorableProcess::Destruct()->PID<%d><%d> calling EndProcess()\n", getpid(), gettid());    
                // Unconditionally KILL !!!
                while (EndProcess() != 0) {
                    ++_cycle;
                    ::SleepMs(1);
                }
                _cycle = ~0;
                printf("WPEFramework::RPC::Communicator::MonitorableProcess::Destruct()->PID<%d><%d> calling _parent.Terminated(this)\n", getpid(), gettid());
                _parent.Terminated(this);
            }
            inline bool Destruct(uint64_t& timeSlot) {
                printf("WPEFramework::RPC::Communicator::MonitorableProcess::Destruct(timeSlot)->PID<%d><%d>\n", getpid(), gettid());    
                if (_time <= timeSlot) {
                    printf("WPEFramework::RPC::Communicator::MonitorableProcess::Destruct(timeSlot)->PID<%d><%d> calling EndProcess()\n", getpid(), gettid());    
                    uint32_t delay = EndProcess();

                    if (delay == 0) {
                        _cycle = ~0;
                        printf("WPEFramework::RPC::Communicator::MonitorableProcess::Destruct(timeSlot)->PID<%d><%d> calling _parent.Terminated(this)\n", getpid(), gettid());    
                        _parent.Terminated(this);
                    }
                    else {
                        timeSlot = Core::Time::Now().Ticks();
                        _time = timeSlot + (delay * 1000 * Core::Time::TicksPerMillisecond);
                        ++_cycle;
                    }
                }

                return (IsTerminated());
            }

            BEGIN_INTERFACE_MAP(MonitorableProcess)
                INTERFACE_ENTRY(IRemoteConnection)
                INTERFACE_ENTRY(IMonitorableProcess)
            END_INTERFACE_MAP

        private:
            // Should return 0 if no more iterations are needed.
            virtual uint32_t EndProcess() = 0;

        private:
            RemoteConnectionMap& _parent;
            const string _callsign;
            uint8_t _cycle;
            uint64_t _time; // in Seconds
        };
        class EXTERNAL Process {
        public:
            Process() = delete;
            Process(Process&&) = delete;
            Process(const Process&) = delete;
            Process& operator=(Process&&) = delete;
            Process& operator=(const Process&) = delete;

            Process(const uint32_t sequenceNumber, const Config& config, const Object& instance)
                : _options(config.HostApplication())
                , _id(0)
            {
                ASSERT(instance.Locator().empty() == false);
                ASSERT(instance.ClassName().empty() == false);
                ASSERT(config.Connector().empty() == false);

                _options.Add(_T("-l")).Add(instance.Locator());
                _options.Add(_T("-c")).Add(instance.ClassName());
                _options.Add(_T("-C")).Add(instance.Callsign());
                _options.Add(_T("-r")).Add(config.Connector());
                _options.Add(_T("-i")).Add(Core::NumberType<uint32_t>(instance.Interface()).Text());
                _options.Add(_T("-x")).Add(Core::NumberType<uint32_t>(sequenceNumber).Text());

                if (instance.Version() != static_cast<uint32_t>(~0)) {
                    _options.Add(_T("-V")).Add(Core::NumberType<uint32_t>(instance.Version()).Text());
                }
                if (instance.User().empty() == false) {
                    _options.Add(_T("-u")).Add(instance.User());
                }
                if (instance.Group().empty() == false) {
                    _options.Add(_T("-g")).Add(instance.Group());
                }
                if (config.PersistentPath().empty() == false) {
                    _options.Add(_T("-p")).Add('"' + config.PersistentPath() + '"');
                }
                if (config.SystemPath().empty() == false) {
                    _options.Add(_T("-s")).Add('"' + config.SystemPath() + '"');
                }
                if (config.DataPath().empty() == false) {
                    _options.Add(_T("-d")).Add('"' + config.DataPath() + '"');
                }
                if (config.ApplicationPath().empty() == false) {
                    _options.Add(_T("-a")).Add('"' + config.ApplicationPath() + '"');
                }
                if (config.VolatilePath().empty() == false) {
                    _options.Add(_T("-v")).Add('"' + config.VolatilePath() + '"');
                }
                if (config.ProxyStubPath().empty() == false) {
                    _options.Add(_T("-m")).Add('"' + config.ProxyStubPath() + '"');
                }
                if (config.PostMortemPath().empty() == false) {
                    _options.Add(_T("-P")).Add('"' + config.PostMortemPath() + '"');
                }
                if (instance.SystemRootPath().empty() == false) {
                    _systemRootPath = instance.SystemRootPath();
                    _options.Add(_T("-S")).Add('"' + instance.SystemRootPath() + '"');
                }
                if (instance.Threads() > 1) {
                    _options.Add(_T("-t")).Add(Core::NumberType<uint8_t>(instance.Threads()).Text());
                }
                _priority = instance.Priority();
            }
            const string& Command() const
            {
                return (_options.Command());
            }
            Core::Process::Options::Iterator Options() const
            {
                return (_options.Get());
            }
            uint32_t Launch(uint32_t& id)
            {
                string oldLDLibraryPaths;
                _ldLibLock.Lock();
                if (_systemRootPath.empty() == false) {

                    string newLDLibraryPaths;
                    Core::SystemInfo::GetEnvironment(_T("LD_LIBRARY_PATH"), oldLDLibraryPaths);

                    PopulateLDLibraryPaths(oldLDLibraryPaths, newLDLibraryPaths);

                    Core::SystemInfo::SetEnvironment(_T("LD_LIBRARY_PATH"), newLDLibraryPaths, true);
                    TRACE_L1("Populated New LD_LIBRARY_PATH : %s", newLDLibraryPaths.c_str());
                }
                printf("WPEFramework::RPC::Communicator::Process::Launch(()->PID<%d><%d> calling Core::Process fork(false)\n", getpid(), gettid());
                // Start the external process launch..
                Core::Process fork(false);
                printf("WPEFramework::RPC::Communicator::Process::Launch(()->PID<%d><%d> calling fork.Launch(_options, &id)\n", getpid(), gettid());
                uint32_t result = fork.Launch(_options, &id);
                _id = id;

                //restore the original value
                if (_systemRootPath.empty() == false) {
                    Core::SystemInfo::SetEnvironment(_T("LD_LIBRARY_PATH"), oldLDLibraryPaths, true);

                }
                _ldLibLock.Unlock();

                if ((result == Core::ERROR_NONE) && (_priority != 0)) {
                    Core::ProcessInfo newProcess(id);
                    newProcess.Priority(newProcess.Priority() + _priority);
                }

                return (result);
            }
            Core::process_t Id() const {
                return(_id);
            }

        private:
            const std::vector<string>& DynamicLoaderPaths() const;
            void PopulateLDLibraryPaths(const string& oldLDLibraryPaths, string& newLDLibraryPaths) const {
                // Read currently added LD_LIBRARY_PATH to prefix with _systemRootPath
                if (oldLDLibraryPaths.empty() != true) {
                    size_t start = 0;
                    size_t end = oldLDLibraryPaths.find(':');
                    do {
                        newLDLibraryPaths += _systemRootPath;
                        newLDLibraryPaths += oldLDLibraryPaths.substr(start,
                            ((end != string::npos) ? (end - start + 1) : end));
                        start = end;
                        if (end != string::npos) {
                            start++;
                            end = oldLDLibraryPaths.find(':', start);
                        }
                    } while (start != string::npos);
                }

                const std::vector<string>& loaderPaths = DynamicLoaderPaths();
                for (const auto& loaderPath : loaderPaths) {
                    newLDLibraryPaths += (newLDLibraryPaths.empty() != true) ? ":" : "";
                    newLDLibraryPaths += _systemRootPath + loaderPath;
                }
            }

        private:
            Core::Process::Options _options;
            int8_t _priority;
            string _systemRootPath;
            Core::process_t _id;
            static Core::CriticalSection _ldLibLock;
        };

    private:
        class EXTERNAL LocalProcess : public MonitorableProcess {
        public:
            friend class Core::Service<LocalProcess>;

            LocalProcess() = delete;
            LocalProcess(LocalProcess&&) = delete;
            LocalProcess(const LocalProcess&) = delete;
            LocalProcess& operator=(LocalProcess&&) = delete;
            LocalProcess& operator=(const LocalProcess&) = delete;

            LocalProcess(const Config& config, const Object& instance, RemoteConnectionMap& parent)
                : MonitorableProcess(instance.Callsign(), parent)
                , _id(0)
                , _process(RemoteConnection::Id(), config, instance)
            {
                printf("WPEFramework::RPC::Communicator::LocalProcess::LocalProcess()->PID<%d><%d> Constructor MonitorableProcess(instance.Callsign(), parent), _process()\n", getpid(), gettid());
            }
            ~LocalProcess() override = default;

        public:
            uint32_t Launch() override
            {
                printf("Communicator::LocalProcess::Launch()->PID<%d> \n", getpid());
                return (_process.Launch(_id));
            }
            const string& Command() const
            {
                printf("Communicator::LocalProcess::Command()->PID<%d> \n", getpid());
                return (_process.Command());
            }
            Core::Process::Options::Iterator Options() const
            {
                printf("Communicator::LocalProcess::Options()->PID<%d> \n", getpid());
                return (_process.Options());
            }
            Core::instance_id ParentPID() const override {
                return (static_cast<Core::instance_id>(_id));
            }
            uint32_t EndProcess() override {
                uint32_t nextinterval = 0;
                Core::Process process(false, _process.Id());

                printf("Communicator::LocalProcess::EndProcess()->PID<%d> \n", getpid());
                if (process.IsActive() != false) {
                    switch (Cycle()) {
                    case 0:
                        printf("Communicator::LocalProcess::EndProcess(case 0)-> calling process.Kill(false) \n");
                        process.Kill(false);
                        nextinterval = Communicator::SoftKillCheckWaitTime();
                        break;
                    default:
                        printf("Communicator::LocalProcess::EndProcess(default)-> calling process.Kill(true) \n");
                        process.Kill(true);
                        nextinterval = Communicator::HardKillCheckWaitTime();
                        break;
                    }
                }
                return nextinterval;
            }

            void PostMortem() override;
            void Terminate() override;
            uint32_t RemoteId() const override;

            BEGIN_INTERFACE_MAP(LocalProcess)
                INTERFACE_ENTRY(IRemoteConnection)
                INTERFACE_ENTRY(IMonitorableProcess)
            END_INTERFACE_MAP

        private:
            uint32_t _id;
            Process _process;
        };

#ifdef PROCESSCONTAINERS_ENABLED

        class EXTERNAL ContainerProcess : public MonitorableProcess {
        private:
            class ContainerConfig : public Core::JSON::Container {
            public:
                ContainerConfig(ContainerConfig&&) = delete;
                ContainerConfig(const ContainerConfig&) = delete;
                ContainerConfig& operator=(ContainerConfig&&) = delete;
                ContainerConfig& operator=(const ContainerConfig&) = delete;

                ContainerConfig()
                    : Core::JSON::Container()
#ifdef __DEBUG__
                    , ContainerPath()
#endif
                {
#ifdef __DEBUG__
                    Add(_T("containerpath"), &ContainerPath);
#endif
                }
                ~ContainerConfig() = default;

#ifdef __DEBUG__
                Core::JSON::String ContainerPath;
#endif
            };

        public:
            friend class Core::Service<ContainerProcess>;

            ContainerProcess(ContainerProcess&&) = delete;
            ContainerProcess(const ContainerProcess&) = delete;
            ContainerProcess& operator=(ContainerProcess&&) = delete;
            ContainerProcess& operator=(const ContainerProcess&) = delete;

            ContainerProcess(const Config& baseConfig, const Object& instance, RemoteConnectionMap& parent)
                : MonitorableProcess(instance.Callsign(), parent)
                , _process(RemoteConnection::Id(), baseConfig, instance)
            {
                ProcessContainers::IContainerAdministrator& admin = ProcessContainers::IContainerAdministrator::Instance();

                std::vector<string> searchpaths(3);
                searchpaths[0] = baseConfig.VolatilePath();
                searchpaths[1] = baseConfig.PersistentPath();
                searchpaths[2] = baseConfig.DataPath();

#ifdef __DEBUG__
                ContainerConfig config;
                config.FromString(instance.Configuration());

                if (config.ContainerPath.IsSet() == true) {
                    searchpaths.emplace(searchpaths.cbegin(), config.ContainerPath.Value());
                }

#endif

                Core::IteratorType<std::vector<string>, const string> searchpathsit(searchpaths);

                string volatilecallsignpath(baseConfig.VolatilePath() + instance.Callsign() + _T('/'));
                _container = admin.Container(instance.Callsign(), searchpathsit, volatilecallsignpath, instance.Configuration());
            }

            ~ContainerProcess() override
            {
                if (_container != nullptr) {
                    _container->Release();
                }
            }

        public:
            uint32_t Launch() override
            {
                uint32_t result = Core::ERROR_GENERAL;

                if (_container != nullptr) {

                    // Note: replace below code with something more efficient when Iterators redesigned
                    Core::Process::Options::Iterator it(_process.Options());

                    std::vector<string> params;
                    while (it.Next() == true) {
                        params.emplace_back(it.Current());
                    }

                    Core::IteratorType<std::vector<string>, const string> temp(params);
                    if (_container->Start(_process.Command(), temp) == true) {
                        result = Core::ERROR_NONE;
                    }
                }

                return result;
            }
            Core::instance_id ParentPID() const override {
                return (static_cast<Core::instance_id>(_container->Pid()));
            }
            uint32_t EndProcess() override {
                uint32_t nextinterval = 0;
                Core::Process process(static_cast<uint32_t>(_container->Pid()));

                if (((process.Id() != 0) && (process.IsActive() == true)) || ((process.Id() == 0) && (_container->IsRunning() == true))) {
                    switch (Cycle()) {
                    case 0: if (process.Id() != 0) {
                                process.Kill(false);
                            }
                            else {
                                _container->Stop(0);
                            }
                            nextinterval = Communicator::SoftKillCheckWaitTime();
                            break;
                    case 1: if (process.Id() != 0) {
                                process.Kill(true);
                                nextinterval = Communicator::HardKillCheckWaitTime();
                            }
                            else {
                                ASSERT(false);
                                nextinterval = 0;
                            }
                            break;
                    case 2: _container->Stop(0);
                            nextinterval = 5;
                            break;
                    default:
                        // This should not happen. This is a very stubbern process. Can not be killed.
                        ASSERT(false);
                        break;
                    }
                }
                return nextinterval;
            }
            uint32_t RemoteId() const override
            {
                return _container->Pid();
            }

            void PostMortem() override;
            void Terminate() override;

            BEGIN_INTERFACE_MAP(ContainerProcess)
                INTERFACE_ENTRY(IRemoteConnection)
                INTERFACE_ENTRY(IMonitorableProcess)
            END_INTERFACE_MAP

        private:
            ProcessContainers::IContainer* _container;
            Process _process;
        };

#endif

    private:
        class EXTERNAL RemoteConnectionMap {
        private:
            class Info {
            public:
                Info() = delete;
                Info(Info&&) = delete;
                Info(const Info&) = delete;
                Info& operator= (Info&&) = delete;
                Info& operator= (const Info&) = delete;

                Info(Core::Event& event, const uint32_t id)
                    : _event(event)
                    , _id(id)
                    , _interface(nullptr) {
                }
                ~Info() = default;

            public:
                inline void Implementation(const Core::ProxyType<Core::IPCChannel>& channel, const Core::instance_id& implementation) {

                    ASSERT(_interface == nullptr);
                    printf("Communicator::RemoteConnectionMap::Implementation()->PID<%d><%d> \n", getpid(), gettid());

                    printf("Communicator::RemoteConnectionMap::Implementation()->PID<%d><%d> implementation<0x%x> _id<%d> _interface<%d> calling  RPC::Administrator::Instance().ProxyInstance()\n", getpid(), gettid(), implementation, _id, _interface);
                    // Get the interface pointer that was stored during the triggering of the event...
                    // It is reference counted so it has to be dereferenced by the caller.
                    RPC::Administrator::Instance().ProxyInstance(channel, implementation, true, _id, _interface);
                    printf("Communicator::RemoteConnectionMap::Implementation()->PID<%d><%d> implementation<0x%x> _id<%d> _interface<%d> calling  _event.SetEvent()\n", getpid(), gettid(), implementation, _id, _interface);
                    _event.SetEvent();
                }
                inline void* Interface()
                {
                    return (_interface);
                }

            private:
                Core::Event& _event;
                uint32_t _id;
                void* _interface;
            };

            using RemoteConnections = std::unordered_map<uint32_t, RemoteConnection*>;
            using Observers = std::vector< RPC::IRemoteConnection::INotification*>;
            using Announcements = std::unordered_map<uint32_t, Info>;

        public:
            RemoteConnectionMap(RemoteConnectionMap&&) = delete;
            RemoteConnectionMap(const RemoteConnectionMap&) = delete;
            RemoteConnectionMap& operator=(const RemoteConnectionMap&) = delete;

            RemoteConnectionMap(Communicator& parent)
                : _adminLock()
                , _announcements()
                , _connections()
                , _parent(parent)
            {
                printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::RemoteConnectionMap()->PID<%d><%d> Constructor\n", getpid(), gettid());
            }
            virtual ~RemoteConnectionMap()
            {
                // All observers should have unregistered before this map get's destroyed !!!
                ASSERT(_observers.size() == 0);
                printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::~RemoteConnectionMap()->PID<%d><%d> Destructor\n", getpid(), gettid());
                while (_observers.size() != 0) {
                    _observers.back()->Release();
                    _observers.pop_back();
                }

                // All connections must be terminated if we end up here :-)
                ASSERT(_connections.size() == 0);
                printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::~RemoteConnectionMap()->PID<%d><%d> Calling Destroy()\n", getpid(), gettid());
                Destroy();
            }

        public:
            inline void Register(RPC::IRemoteConnection::INotification* sink)
            {
                ASSERT(sink != nullptr);
                printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::Register(_observers)->PID<%d><%d> \n", getpid(), gettid());
                if (sink != nullptr) {

                    _adminLock.Lock();

                    ASSERT(std::find(_observers.begin(), _observers.end(), sink) == _observers.end());

                    sink->AddRef();
                    _observers.push_back(sink);

                    RemoteConnections::iterator index(_connections.begin());

                    printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::Register(_connections)->PID<%d><%d> Report all Active Processes sink->Activated() \n", getpid(), gettid());
                    // Report all Active Processes..
                    while (index != _connections.end()) {
                        if (index->second->IsOperational() == true) {
                            sink->Activated(&(*(index->second)));
                        }
                        index++;
                    }

                    _adminLock.Unlock();
                }
            }
            inline void Unregister(const RPC::IRemoteConnection::INotification* sink)
            {
                ASSERT(sink != nullptr);
                printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::Unregister(_observers)->PID<%d><%d> \n", getpid(), gettid());
                if (sink != nullptr) {

                    _adminLock.Lock();

                    Observers::iterator index(std::find(_observers.begin(), _observers.end(), sink));

                    ASSERT(index != _observers.end());

                    if (index != _observers.end()) {
                        (*index)->Release();
                        _observers.erase(index);
                    }

                    _adminLock.Unlock();
                }
            }
            inline void* Create(uint32_t& id, const Object& instance, const Config& config, const uint32_t waitTime)
            {
                void* interfaceReturned = nullptr;

                id = 0;

                _adminLock.Lock();
                printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::Create()->PID<%d><%d> instance<0x%8x> calling _parent.CreateStarter(config, instance)\n", getpid(), gettid(), instance);
                RemoteConnection* result = _parent.CreateStarter(config, instance);

                ASSERT(result != nullptr);

                if (result != nullptr) {

                    Core::Event trigger(false, true);

                    // A reference for putting it in the list...
                    result->AddRef();
                    printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::Create(_connections)->PID<%d><%d> result->Id()<%d>result->RemoteId<%d>\n", getpid(), gettid(), result->Id(), result->RemoteId());
                    // We expect an announce interface message now...
                    _connections.insert(std::pair<uint32_t, RemoteConnection*>(result->Id(), result));
                    auto locator = _announcements.emplace(std::piecewise_construct,
                        std::forward_as_tuple(result->Id()),
                        std::forward_as_tuple(trigger, instance.Interface()));

                    _adminLock.Unlock();
                    printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::Create()->PID<%d><%d> calling result->Launch() \n", getpid(), gettid());
                    // Start the process, and....
                    uint32_t launchResult = result->Launch();
                    printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::Create()->PID<%d><%d>waitTime<%d> calling trigger.Lock(waitTime) \n", getpid(), gettid(), waitTime);
                    // wait for the announce message to be exchanged
                    if ((launchResult == Core::ERROR_NONE) && (trigger.Lock(waitTime) == Core::ERROR_NONE)) {

                        interfaceReturned = locator.first->second.Interface();

                        if (interfaceReturned != nullptr) {

                            id = result->Id();
                        }

                    } else {
                        printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::Create()->PID<%d><%d> Seems we could not start the application result->Terminate()\n", getpid(), gettid());
                        // Seems we could not start the application. Cleanout
                        result->Terminate();
                    }

                    _adminLock.Lock();
                    printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::Create()->PID<%d><%d> _announcements.erase(locator.first)\n", getpid(), gettid());
                    // Kill the Event registration. We are no longer interested in what will be hapening..
                    _announcements.erase(locator.first);

                    printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::Create()->PID<%d><%d> result->Release() \n", getpid(), gettid());
                    result->Release();
                }
                _adminLock.Unlock();
                printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::Create()->PID<%d><%d> return interfaceReturned\n", getpid(), gettid());
                return (interfaceReturned);
            }
            inline void Closed(const uint32_t id)
            {
                // First do an activity check on all processes registered.
                _adminLock.Lock();
                printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::Closed()->PID<%d><%d> id<%d>\n", getpid(), gettid(), id);
                RemoteConnections::iterator index(_connections.find(id));

                if (index == _connections.end()) {

                    _adminLock.Unlock();

                } else {
                    Communicator::RemoteConnection* connection = index->second;
                    printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::Closed()->PID<%d><%d> calling connection->AddRef()\n", getpid(), gettid());
                    connection->AddRef();

                    printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::Closed()->PID<%d><%d> calling index->second->Channel()\n", getpid(), gettid());
                    // Remove any channel associated, we had.
                    Core::ProxyType<Core::IPCChannel> destructed = index->second->Channel();
                    printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::Closed()->PID<%d><%d> calling index->second->Close()\n", getpid(), gettid());
                    index->second->Close();

                    Observers::iterator observer(_observers.begin());
                    printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::Closed()->PID<%d><%d> calling _parent.Closed(destructed)\n", getpid(), gettid());
                    _parent.Closed(destructed);

                    while (observer != _observers.end()) {
                        printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::Closed()->PID<%d><%d> calling (*observer)->Deactivated(index->second)\n", getpid(), gettid());
                        (*observer)->Deactivated(index->second);
                        observer++;
                    }
                    printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::Closed()->PID<%d><%d> calling index->second->Terminate()\n", getpid(), gettid());
                    // Don't forget to close on our side as well, if it is not already closed....
                    index->second->Terminate();

                    printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::Closed()->PID<%d><%d> calling index->second->Release()\n", getpid(), gettid());
                    // Release this entry, do not wait till it get's overwritten.
                    index->second->Release();
                    printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::Closed()->PID<%d><%d> calling _connections.erase(index)\n", getpid(), gettid());
                    _connections.erase(index);
                    _adminLock.Unlock();
                    printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::Closed()->PID<%d><%d> calling connection->Release()\n", getpid(), gettid());
                    connection->Release();
                }
            }
            inline Communicator::RemoteConnection* Connection(const uint32_t id)
            {
                Communicator::RemoteConnection* result = nullptr;

                _adminLock.Lock();
                printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::Connection()->PID<%d><%d>id<%d>\n", getpid(), gettid(), id);

                RemoteConnections::iterator index(_connections.find(id));

                if (index != _connections.end()) {
                    result = index->second;
                    printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::Connection()->PID<%d><%d>calling result->AddRef()\n", getpid(), gettid());
                    result->AddRef();
                }

                _adminLock.Unlock();
                printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::Connection()->PID<%d><%d> return result\n", getpid(), gettid());
                return (result);
            }
            inline void Destroy()
            {
                // First do an activity check on all processes registered.
                _adminLock.Lock();
                printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::Destroy()->PID<%d><%d> _connections.size<%d>\n", getpid(), gettid(), _connections.size());
                while (_connections.size() > 0) {
                    TRACE_L1("Forcefully closing open RPC Server connection: %d", _connections.begin()->second->Id());
                    printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::Destroy()->PID<%d><%d>ConnectionId<%d> calling Terminate()\n", getpid(), gettid(), _connections.begin()->second->Id());    
                    _connections.begin()->second->Terminate();
                    printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::Destroy()->PID<%d><%d>ConnectionId<%d> calling Release()\n", getpid(), gettid(), _connections.begin()->second->Id());    
                    _connections.begin()->second->Release();
                    printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::Destroy()->PID<%d><%d> calling erase()\n", getpid(), gettid());    
                    _connections.erase(_connections.begin());
                }

                _adminLock.Unlock();
            }
            void* Announce(Core::ProxyType<Client>& channel, const Data::Init& info, Data::Setup& response)
            {
                void* result = nullptr;

                _adminLock.Lock();
                printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::Announce()->PID<%d><%d>\n", getpid(), gettid());
                response.Action(Data::Output::NONE);

                if (info.IsRequested() == true) {
                    printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::Announce()->PID<%d><%d>calling Request(channel, info)\n", getpid(), gettid());
                    Request(channel, info);
                } else {

                    if (channel->Extension().IsRegistered() == false) {

                        // This is an announce message from a process that wasn't created by us. So typically this is
                        // An RPC client reaching out to an RPC server. The RPCServer does not spawn processes it just
                        // listens for clients requesting service.
                        Communicator::RemoteConnection* remoteConnection = Core::Service<RemoteConnection>::Create<RemoteConnection>(channel, info.Id());

                        channel->Extension().Link(*this, remoteConnection->Id());
                        ASSERT(remoteConnection != nullptr);

                        // Add ref is done during the creation, no need to take another reference unless we also would release it after
                        // insertion :-)
                        _connections.emplace(
                            std::piecewise_construct,
                                std::forward_as_tuple(remoteConnection->Id()),
                                std::forward_as_tuple(remoteConnection));

                        Activated(remoteConnection);
                    }

                    result = Handle(channel, info, response);
                }

                _adminLock.Unlock();
                printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::Announce()->PID<%d><%d> return\n", getpid(), gettid());
                return (result);
            }
            void Terminated(RPC::IRemoteConnection* connection)
            {
                _adminLock.Lock();
                printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::Terminated()->PID<%d><%d> Entered\n", getpid(), gettid());
                Observers::iterator index(_observers.begin());
                while (index != _observers.end()) {
                    printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::Terminated()->PID<%d><%d> calling(*index)->Terminated(connection)\n", getpid(), gettid());
                    (*index)->Terminated(connection);
                    index++;
                }
                _adminLock.Unlock();
            }

        private:
            void Activated(RPC::IRemoteConnection* connection)
            {
                Observers::iterator index(_observers.begin());
                while (index != _observers.end()) {
                    (*index)->Activated(connection);
                    index++;
                }
            }
            void Request(Core::ProxyType<Client>& channel, const Data::Init& info)
            {
               RemoteConnections::iterator index(_connections.find(info.ExchangeId()));

                printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::Request()->PID<%d><%d> Entered \n", getpid(), gettid());
                ASSERT(index != _connections.end());
                ASSERT(index->second->IsOperational() == false);

                printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::Request()->PID<%d><%d> calling index->second->Open() \n", getpid(), gettid());
                // This is when we requested this interface/object to be created, there must be already an
                // administration, it is just not complete.... yet!!!!
                index->second->Open(channel, info.Id());
                channel->Extension().Link(*this, index->second->Id());

                printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::Request()->PID<%d><%d> calling Activated(index->second) \n", getpid(), gettid());
                Activated(index->second);

                auto processConnection = _announcements.find(index->second->Id());

                if (processConnection != _announcements.end()) {

                    Core::ProxyType<Core::IPCChannel> baseChannel(channel);

                    ASSERT(info.Implementation());
                    printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::Request()->PID<%d><%d> calling processConnection->second.Implementation() \n", getpid(), gettid());
                    processConnection->second.Implementation(baseChannel, info.Implementation());
                } else {
                    // No one picks it up, release it..
                    // TODO: Release an object that will never be used...
                }
            }

            void* Handle(Core::ProxyType<Client>& channel, const Data::Init& info, Data::Setup& response)
            {
                Core::ProxyType<Core::IPCChannel> baseChannel(channel);
                ASSERT(baseChannel.IsValid() == true);

                void* result = nullptr;
                printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::Handle()->PID<%d><%d> Entered\n", getpid(), gettid());
                if (info.IsOffer() == true) {

                    Core::instance_id implementation = info.Implementation();
                    ASSERT(implementation != 0);
                    printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::Handle(info.IsOffer() == true)->PID<%d><%d> Administrator::Instance().ProxyInstance()\n", getpid(), gettid());
                    void* realIF = nullptr;

                    ProxyStub::UnknownProxy* base = Administrator::Instance().ProxyInstance(baseChannel, implementation, false, info.InterfaceId(), realIF);
                    ASSERT(base != nullptr);

                    if (base != nullptr) {
                        Core::IUnknown* realIFbase = base->Parent();
                        ASSERT(realIFbase != nullptr);
                        printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::Handle()->PID<%d><%d> _parent.Offer(realIFbase, info.InterfaceId())\n", getpid(), gettid());
                        _parent.Offer(realIFbase, info.InterfaceId());
                        printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::Handle()->PID<%d><%d> base->Complete(response)\n", getpid(), gettid());
                        base->Complete(response);
                    }

                } else if (info.IsRevoke() == true) {

                    Core::instance_id implementation = info.Implementation();
                    ASSERT(implementation != 0);

                    void* realIF = nullptr;
                    printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::Handle(info.IsRevoke() == true)->PID<%d><%d> Administrator::Instance().ProxyFind()\n", getpid(), gettid());
                    ProxyStub::UnknownProxy* base = Administrator::Instance().ProxyFind(baseChannel, implementation, info.InterfaceId(), realIF);

                    if (base != nullptr) {
                        Core::IUnknown* realIFbase = base->Parent();
                        ASSERT(realIFbase != nullptr);
                        printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::Handle(info.IsRevoke() == true)->PID<%d><%d> calling bbase->AddRef()\n", getpid(), gettid());
                        base->AddRef();
                        printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::Handle(info.IsRevoke() == true)->PID<%d><%d> calling _parent.Revoke()\n", getpid(), gettid());
                        _parent.Revoke(realIFbase, info.InterfaceId());
                        printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::Handle(info.IsRevoke() == true)->PID<%d><%d> calling base->Complete(response)\n", getpid(), gettid());
                        base->Complete(response);
                    }

                } else if (info.InterfaceId() != static_cast<uint32_t>(~0)) {

                    printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::Handle(info.InterfaceId() != static_cast<uint32_t>(~0))->PID<%d><%d> calling _parent.Acquire()\n", getpid(), gettid());
                    // See if we have something we can return right away, if it has been requested..
                    result = _parent.Acquire(info.ClassName(), info.InterfaceId(), info.VersionId());

                    if (result != nullptr) {
                        printf("WPEFramework::RPC::Communicator::RemoteConnectionMap::Handle(info.InterfaceId() != static_cast<uint32_t>(~0))->PID<%d><%d> calling Administrator::Instance().RegisterInterface()\n", getpid(), gettid());
                        Administrator::Instance().RegisterInterface(baseChannel, result, info.InterfaceId());
                    }
                }

                return (result);
            }

        private:
            mutable Core::CriticalSection _adminLock;
            Announcements _announcements;
            RemoteConnections _connections;
            Observers _observers;
            Communicator& _parent;
        };

    public:
        class EXTERNAL ChannelLink {
        public:
            ChannelLink() = delete;
            ChannelLink(ChannelLink&&) = delete;
            ChannelLink(const ChannelLink&) = delete;
            ChannelLink& operator=(ChannelLink&&) = delete;
            ChannelLink& operator=(const ChannelLink&) = delete;

            ChannelLink(Core::IPCChannelType<Core::SocketPort, ChannelLink>* channel)
                : _channel(channel->Source())
                , _connectionMap(nullptr)
                , _id(0)
            {
                // We are a composit of the Channel, no need (and do not for cyclic references) not maintain a reference...
                ASSERT(channel != nullptr);
            }
            ~ChannelLink() = default;

        public:
            void Link(RemoteConnectionMap& connectionMap, const uint32_t id)
            {
                printf("WPEFramework::RPC::Communicator::ChannelLink::Link()->PID<%d><%d>connectionMap<>id<%d>\n", getpid(), gettid(), id);
                _connectionMap = &connectionMap;
                _id = id;
            }
            void StateChange()
            {
                printf("WPEFramework::RPC::Communicator::ChannelLink::StateChange()->PID<%d><%d>\n", getpid(), gettid());
                // If the connection closes, we need to clean up....
                if ((_channel.IsOpen() == false) && (_connectionMap != nullptr)) {
                    printf("WPEFramework::RPC::Communicator::ChannelLink::StateChange()->PID<%d><%d>calling _connectionMap->Closed(_id<%d>)\n", getpid(), gettid(), _id);
                    _connectionMap->Closed(_id);
                }
            }
            bool IsRegistered() const
            {
                return (_connectionMap != nullptr);
            }
            uint32_t Id() const
            {
                return _id;
            }

        private:
            // Non ref-counted reference to our parent, of which we are a composit :-)
            Core::SocketPort& _channel;
            RemoteConnectionMap* _connectionMap;
            uint32_t _id;
        };

    private:
        class ChannelServer : public Core::IPCChannelServerType<ChannelLink, true> {
        private:
            using BaseClass = Core::IPCChannelServerType<ChannelLink, true>;

            class AnnounceHandler : public Core::IIPCServer {
            public:
                AnnounceHandler() = delete;
                AnnounceHandler(AnnounceHandler&&) = delete;
                AnnounceHandler(const AnnounceHandler&) = delete;
                AnnounceHandler& operator=(AnnounceHandler&&) = delete;
                AnnounceHandler& operator=(const AnnounceHandler&) = delete;

                AnnounceHandler(ChannelServer& parent)
                    : _parent(parent) {
                }
                ~AnnounceHandler() override = default;

            public:
                void Procedure(Core::IPCChannel& channel, Core::ProxyType<Core::IIPC>& data) override {

                    Core::ProxyType<AnnounceMessage> message(data);
                    printf("WPEFramework::RPC::Communicator::ChannelServer::AnnounceHandler::Procedure()->PID<%d><%d>\n", getpid(), gettid());
                    ASSERT(message.IsValid() == true);
                    ASSERT(dynamic_cast<Client*>(&channel) != nullptr);

                    // Validate if the frame coming in is properly formatted..
                    if (message->Parameters().IsValid() == false) {
                        SYSLOG(Logging::Error, (_T("COMRPC Announce message incorrectly formatted!")));
                    }
                    else {
                        Core::ProxyType<Client> proxyChannel(static_cast<Client&>(channel));

                        string jsonDefaultMessagingSettings;
                        string jsonDefaultWarningReportingSettings;

                        #if defined(WARNING_REPORTING_ENABLED)
                        jsonDefaultWarningReportingSettings = WarningReporting::WarningReportingUnit::Instance().Defaults();
                        #endif
                        printf("WPEFramework::RPC::Communicator::ChannelServer::AnnounceHandler::Procedure()->PID<%d><%d>calling _parent.Announce()\n", getpid(), gettid());
                        void* result = _parent.Announce(proxyChannel, message->Parameters(), message->Response());

                        message->Response().Set(instance_cast<void*>(result), proxyChannel->Extension().Id(), _parent.ProxyStubPath(), jsonDefaultMessagingSettings, jsonDefaultWarningReportingSettings);
                        printf("WPEFramework::RPC::Communicator::ChannelServer::AnnounceHandler::Procedure()->PID<%d><%d>calling channel.ReportResponse(data)\n", getpid(), gettid());
                        // We are done, report completion
                        channel.ReportResponse(data);
                    }
                }

            private:
                ChannelServer& _parent;
            };

        public:
            ChannelServer(ChannelServer&&) = delete;
            ChannelServer(const ChannelServer&) = delete;
            ChannelServer& operator=(ChannelServer&&) = delete;
            ChannelServer& operator=(const ChannelServer&) = delete;

            PUSH_WARNING(DISABLE_WARNING_THIS_IN_MEMBER_INITIALIZER_LIST)
            ChannelServer(
                const Core::NodeId& remoteNode,
                RemoteConnectionMap& processes,
                const string& proxyStubPath)
                : BaseClass(remoteNode, CommunicationBufferSize)
                , _proxyStubPath(proxyStubPath)
                , _connections(processes) {
                    printf("WPEFramework::RPC::Communicator::ChannelServer::ChannelServer()->PID<%d><%d>\n", getpid(), gettid());
                BaseClass::Register(InvokeMessage::Id(), Core::ProxyType<Core::IIPCServer>(Core::ProxyType<InvokeHandler>::Create()));
                BaseClass::Register(AnnounceMessage::Id(), Core::ProxyType<Core::IIPCServer>(Core::ProxyType<AnnounceHandler>::Create(*this)));
            }
            ChannelServer(
                const Core::NodeId& remoteNode,
                RemoteConnectionMap& processes,
                const string& proxyStubPath,
                const Core::ProxyType<Core::IIPCServer>& handler)
                : BaseClass(remoteNode, CommunicationBufferSize)
                , _proxyStubPath(proxyStubPath)
                , _connections(processes) {
                BaseClass::Register(InvokeMessage::Id(), handler);
                BaseClass::Register(AnnounceMessage::Id(), Core::ProxyType<Core::IIPCServer>(Core::ProxyType<AnnounceHandler>::Create(*this)));
            }
POP_WARNING()

            ~ChannelServer() {
                BaseClass::Unregister(AnnounceMessage::Id());
                BaseClass::Unregister(InvokeMessage::Id());
            }

        public:
            inline const string& ProxyStubPath() const {
                return (_proxyStubPath);
            }

        private:
            inline void* Announce(Core::ProxyType<Client>& channel, const Data::Init& info, Data::Setup& response)
            {
                // We are in business, register the process with this channel.
                return (_connections.Announce(channel, info, response));
            }

        private:
            const string _proxyStubPath;
            RemoteConnectionMap& _connections;
        };

    protected:
        virtual RemoteConnection* CreateStarter(const Config& config, const Object& instance)
        {
            RemoteConnection* result = nullptr;

            if (instance.Type() == Object::HostType::LOCAL) {
                result = Core::Service<LocalProcess>::Create<RemoteConnection>(config, instance, _connectionMap);
            }
            else if (instance.Type() == Object::HostType::CONTAINER) {
#ifdef PROCESSCONTAINERS_ENABLED
                result = Core::Service<ContainerProcess>::Create<RemoteConnection>(config, instance, _connectionMap);
#else
                SYSLOG(Logging::Error, (_T("Cannot create Container process for %s, this version was not build with Container support"), instance.ClassName().c_str()));
#endif
            }

            return result;
        }
        void LoadProxyStubs(const string& pathName);

    public:
        Communicator() = delete;
        Communicator(Communicator&&) = delete;
        Communicator(const Communicator&) = delete;
        Communicator& operator=(Communicator&&) = delete;
        Communicator& operator=(const Communicator&) = delete;

        Communicator(
            const Core::NodeId& node,
            const string& proxyStubPath);
        Communicator(
            const Core::NodeId& node,
            const string& proxyStubPath,
            const Core::ProxyType<Core::IIPCServer>& handler);
        virtual ~Communicator();

    public:
        // void action(const Client& client)
        template<typename ACTION>
        void Visit(ACTION&& action) {
            _ipcServer.Visit(action);
        }
        // void action(const Client& client)
        template<typename ACTION>
        void Visit(ACTION&& action) const {
            _ipcServer.Visit(action);
        }
        inline bool IsListening() const
        {
            return (_ipcServer.IsListening());
        }
        inline uint32_t Open(const uint32_t waitTime)
        {
            return (_ipcServer.Open(waitTime));
        }
        inline uint32_t Close(const uint32_t waitTime)
        {
            return (_ipcServer.Close(waitTime));
        }
        inline const string& Connector() const
        {
            return (_ipcServer.Connector());
        }
        inline const string& ProxyStubPath() const
        {
            return (_ipcServer.ProxyStubPath());
        }
        void ForcedDestructionTimes(const uint8_t softKillCheckWaitTime, const uint8_t hardKillCheckWaitTime)
        {
            _softKillCheckWaitTime = softKillCheckWaitTime;
            _hardKillCheckWaitTime = hardKillCheckWaitTime;
        }
        static uint8_t SoftKillCheckWaitTime()
        {
            return _softKillCheckWaitTime;
        }
        static uint8_t HardKillCheckWaitTime()
        {
            return _hardKillCheckWaitTime;
        }

        inline void Register(RPC::IRemoteConnection::INotification* sink)
        {
            printf("WPEFramework::RPC::Communicator::Register()->PID<%d><%d> calling _connectionMap.Register()\n", getpid(), gettid());
            _connectionMap.Register(sink);
        }
        inline void Unregister(const RPC::IRemoteConnection::INotification* sink)
        {
            printf("WPEFramework::RPC::Communicator::Unregister()->PID<%d><%d> calling _connectionMap.Unregister()\n", getpid(), gettid());
            _connectionMap.Unregister(sink);
        }
        inline IRemoteConnection* Connection(const uint32_t id)
        {
            printf("WPEFramework::RPC::Communicator::Connection()->PID<%d><%d> _connectionId<%d> calling _connectionMap.Connection()\n", getpid(), gettid(), id);
            return (_connectionMap.Connection(id));
        }
        inline void* Create(uint32_t& pid, const Object& instance, const Config& config, const uint32_t waitTime)
        {
            printf("WPEFramework::RPC::Communicator::Create()->PID<%d><%d> calling _connectionMap.Create()\n", getpid(), gettid());
            return (_connectionMap.Create(pid, instance, config, waitTime));
        }
        void Destroy()
        {
            printf("WPEFramework::RPC::Communicator::Destroy()->PID<%d><%d>calling _connectionMap.Destroy()\n");
            _connectionMap.Destroy();
        }
        void Destroy(const uint32_t id);

    private:
        void Closed(const Core::ProxyType<Core::IPCChannel>& channel)
        {
            Administrator::Proxies deadProxies;
            printf("WPEFramework::RPC::Communicator::Closed()->PID<%d><%d> calling RPC::Administrator::Instance().DeleteChannel()\n", getpid(), gettid());
            RPC::Administrator::Instance().DeleteChannel(channel, deadProxies);
                
            std::vector<ProxyStub::UnknownProxy*>::const_iterator loop(deadProxies.begin());
            while (loop != deadProxies.end()) {
                Dangling((*loop)->Parent(), (*loop)->InterfaceId());

                // To avoid race conditions, the creation of the deadProxies took a reference
                // on the interfaces, we presented here. Do not forget to release this reference.
                (*loop)->Parent()->Release();
                loop++;
            }
        }
        virtual void* Acquire(const string& /* className */, const uint32_t /* interfaceId */, const uint32_t /* version */)
        {
            return (nullptr);
        }
        virtual void Offer(Core::IUnknown* /* remote */, const uint32_t /* interfaceId */) {
        }
        virtual void Revoke(const Core::IUnknown* /* remote */, const uint32_t /* interfaceId */) {
        }
        virtual void Dangling(const Core::IUnknown* /* remote */, const uint32_t /* interfaceId */) {
        }

    private:
        RemoteConnectionMap _connectionMap;
        ChannelServer _ipcServer;
        static uint8_t _softKillCheckWaitTime;
        static uint8_t _hardKillCheckWaitTime;
    };

    class EXTERNAL CommunicatorClient : public Core::IPCChannelClientType<Core::Void, false, true>, public Core::IDispatchType<Core::IIPC> {
    private:
        typedef Core::IPCChannelClientType<Core::Void, false, true> BaseClass;

        class AnnounceHandler : public Core::IIPCServer {
        public:
            AnnounceHandler() = delete;
            AnnounceHandler(AnnounceHandler&&) = delete;
            AnnounceHandler(const AnnounceHandler&) = delete;
            AnnounceHandler& operator=(AnnounceHandler&&) = delete;
            AnnounceHandler& operator=(const AnnounceHandler&) = delete;

            AnnounceHandler(CommunicatorClient& parent)
                : _parent(parent) {
            }
            ~AnnounceHandler() override = default;

        public:
            void Procedure(IPCChannel& channel, Core::ProxyType<Core::IIPC>& data) override
            {
                // Oke, see if we can reference count the IPCChannel
                Core::ProxyType<Core::IPCChannel> refChannel(channel);
                Core::ProxyType<RPC::AnnounceMessage> message(data);

                ASSERT(refChannel.IsValid());

                const string className(message->Parameters().ClassName());
                const uint32_t interfaceId(message->Parameters().InterfaceId());
                const uint32_t versionId(message->Parameters().VersionId());

                printf("CommunicatorClient::AnnounceHandler::Procedure()->PID<%d> className<%s>interfaceId<%d> calling _parent.Acquire() get implementation\n", Core::ProcessInfo().Id(), className.c_str(), interfaceId);
                Core::instance_id implementation = instance_cast<void*>(_parent.Acquire(className, interfaceId, versionId));
                message->Response().Implementation(implementation);

                channel.ReportResponse(data);
            }

        private:
            CommunicatorClient& _parent;
        };

    public:
        CommunicatorClient() = delete;
        CommunicatorClient(CommunicatorClient&&) = delete;
        CommunicatorClient(const CommunicatorClient&) = delete;
        CommunicatorClient& operator=(CommunicatorClient&&) = delete;
        CommunicatorClient& operator=(const CommunicatorClient&) = delete;

        CommunicatorClient(
            const Core::NodeId& remoteNode);
        CommunicatorClient(
            const Core::NodeId& remoteNode,
            const Core::ProxyType<Core::IIPCServer>& handler);
        ~CommunicatorClient();

    public:
        inline uint32_t ConnectionId() const {
            return _connectionId;
        }

        // Open a communication channel with this process, no need for an initial exchange
        uint32_t Open(const uint32_t waitTime);

        template <typename INTERFACE>
        inline INTERFACE* Open(const string& className, const uint32_t version = static_cast<uint32_t>(~0), const uint32_t waitTime = CommunicationTimeOut)
        {
            INTERFACE* result = nullptr;
            printf(" WPEFramework::RPC::CommunicatorClient::Open()->PID<%d><%d> INTERFACE<%s> INTERFACE::ID<%d> className<%s>\n", getpid(), gettid(), (Core::ClassNameOnly(typeid(INTERFACE).name()).Text()).c_str(), INTERFACE::ID, className.c_str());
            if (Open(waitTime, className, INTERFACE::ID, version) == Core::ERROR_NONE) {
                // Oke we could open the channel, lets get the interface
                result = WaitForCompletion<INTERFACE>(waitTime);
            }

            return (result);
        }

        // Open and offer the requested interface (Applicable if the WPEProcess starts the RPCClient)
        uint32_t Open(const uint32_t waitTime, const uint32_t interfaceId, void* implementation, const uint32_t exchangeId);

        template <typename INTERFACE>
        INTERFACE* Acquire(const uint32_t waitTime, const string& className, const uint32_t versionId)
        {
            INTERFACE* result(nullptr);
            printf(" WPEFramework::RPC::CommunicatorClient::Acquire()->PID<%d><%d> className<%s>\n", getpid(), gettid(), className.c_str());
            if (BaseClass::IsOpen() == true) {

                _announceMessage.Parameters().Set(Core::ProcessInfo().Id(), className, INTERFACE::ID, versionId);

                printf(" WPEFramework::RPC::CommunicatorClient::Acquire()->PID<%d><%d> className<%s> calling BaseClass::Invoke()\n", getpid(), gettid(), className.c_str());
                // Lock event until Dispatch() sets it.
                if (BaseClass::Invoke(Core::ProxyType<RPC::AnnounceMessage>(_announceMessage), waitTime) == Core::ERROR_NONE) {

                    ASSERT(_announceMessage.Parameters().InterfaceId() == INTERFACE::ID);
                    ASSERT(_announceMessage.Parameters().Implementation() == 0);

                    Core::instance_id implementation(_announceMessage.Response().Implementation());
                    printf(" WPEFramework::RPC::CommunicatorClient::Acquire()->PID<%d><%d> className<%s> cimplementation<%lld>\n", getpid(), gettid(), className.c_str(), implementation);
                    if (implementation) {
                        Core::ProxyType<Core::IPCChannel> baseChannel(*this);

                        ASSERT(baseChannel.IsValid() == true);
                        printf(" WPEFramework::RPC::CommunicatorClient::Acquire()->PID<%d><%d> className<%s> calling Administrator::Instance().ProxyInstance() \n", getpid(), gettid(), className.c_str());
                        Administrator::Instance().ProxyInstance(baseChannel, implementation, true, result);
                    }
                }
            }

            return (result);
        }
        template <typename INTERFACE>
        inline uint32_t Offer(INTERFACE* offer, const uint32_t version = static_cast<uint32_t>(~0), const uint32_t waitTime = CommunicationTimeOut)
        {
            uint32_t result(Core::ERROR_NONE);
            printf(" WPEFramework::RPC::CommunicatorClient::Offer()->PID<%d><%d>\n", getpid(), gettid());
            if (BaseClass::IsOpen() == true) {

                _announceMessage.Parameters().Set(Core::ProcessInfo().Id(), INTERFACE::ID, instance_cast<void*>(offer), Data::Init::OFFER);

                Core::ProxyType<Core::IPCChannel> baseChannel(*this);
                ASSERT(baseChannel.IsValid() == true);

                // Ensure the offer instance is known to security if it is used before the call with cached AddRef returns.
                const RPC::InstanceRecord localInstances[] = { { RPC::instance_cast(offer), INTERFACE::ID }, { 0, 0 } };
                baseChannel->CustomData(localInstances);
                printf(" WPEFramework::RPC::CommunicatorClient::Offer()->PID<%d><%d> calling BaseClass::Invoke()\n", getpid(), gettid());
                BaseClass::Invoke(Core::ProxyType<RPC::AnnounceMessage>(_announceMessage), waitTime);

                // Lock event until Dispatch() sets it.
                if (_announceEvent.Lock(waitTime) == Core::ERROR_NONE) {

                    ASSERT(_announceMessage.Parameters().InterfaceId() == INTERFACE::ID);
                    ASSERT(_announceMessage.Parameters().Implementation() != 0);

                    const Data::Output::mode action = _announceMessage.Response().Action();
                    ASSERT(action != Data::Output::mode::CACHED_RELEASE);

                    if (action == Data::Output::mode::CACHED_ADDREF) {
                        Administrator::Instance().AddRef(baseChannel, offer, INTERFACE::ID);
                    }
                } else {
                    result = Core::ERROR_BAD_REQUEST;
                }

                baseChannel->CustomData(nullptr);
            }

            return (result);
        }
        template <typename INTERFACE>
        inline uint32_t Revoke(INTERFACE* offer, const uint32_t version = static_cast<uint32_t>(~0), const uint32_t waitTime = CommunicationTimeOut)
        {
            uint32_t result(Core::ERROR_NONE);
            printf(" WPEFramework::RPC::CommunicatorClient::Revoke()->PID<%d><%d> INTERFACE<%s>INTERFACE::ID<%d>offer<%p> calling BaseClass::Invoke()\n", getpid(), gettid(), (Core::ClassNameOnly(typeid(INTERFACE).name()).Text()).c_str(), INTERFACE::ID, offer);
            if (BaseClass::IsOpen() == true) {

                _announceMessage.Parameters().Set(Core::ProcessInfo().Id(), INTERFACE::ID, instance_cast<void*>(offer), Data::Init::REVOKE);

                Core::ProxyType<Core::IPCChannel> baseChannel(*this);
                ASSERT(baseChannel.IsValid() == true);

                const RPC::InstanceRecord localInstances[] = { { RPC::instance_cast(offer), INTERFACE::ID }, { 0, 0 } };
                baseChannel->CustomData(localInstances);

                BaseClass::Invoke(Core::ProxyType<RPC::AnnounceMessage>(_announceMessage), waitTime);

                // Lock event until Dispatch() sets it.
                if (_announceEvent.Lock(waitTime) == Core::ERROR_NONE) {

                    ASSERT(_announceMessage.Parameters().InterfaceId() == INTERFACE::ID);
                    ASSERT(_announceMessage.Parameters().Implementation() != 0);

                    const Data::Output::mode action = _announceMessage.Response().Action();

                    if (action == Data::Output::mode::CACHED_RELEASE) {
                        Administrator::Instance().Release(baseChannel, offer, INTERFACE::ID, 1);
                    }
                    else if (action == Data::Output::mode::CACHED_ADDREF) {
                        Administrator::Instance().AddRef(baseChannel, offer, INTERFACE::ID);
                    }
                } else {
                    result = Core::ERROR_BAD_REQUEST;
                }

                baseChannel->CustomData(nullptr);
            }

            return (result);
        }

        uint32_t Close(const uint32_t waitTime);

        virtual void* Acquire(const string& className, const uint32_t interfaceId, const uint32_t versionId)
        {
            Core::Library emptyLibrary;

            printf(" WPEFramework::RPC::CommunicatorClient::Acquire()->PID<%d><%d>\n", getpid(), gettid());
            printf(" WPEFramework::RPC::CommunicatorClient::Acquire()->PID<%d><%d>  className.c_str<%s> interfaceId<%d>Core::ServiceAdministrator::Instance().Instantiate()\n", getpid(), gettid(),  className.c_str(), interfaceId);
            // Allright, respond with the interface.
            void* result = Core::ServiceAdministrator::Instance().Instantiate(emptyLibrary, className.c_str(), versionId, interfaceId);

            if (result != nullptr) {
                Core::ProxyType<Core::IPCChannel> baseChannel(*this);
                printf(" WPEFramework::RPC::CommunicatorClient::Acquire()->PID<%d><%d> Administrator::Instance().RegisterInterface()\n", getpid(), gettid());
                Administrator::Instance().RegisterInterface(baseChannel, result, interfaceId);
            }

            return (result);
        }

    private:
        // Open and request an interface from the other side on the announce message (Any RPC client uses this)
        uint32_t Open(const uint32_t waitTime, const string& className, const uint32_t interfaceId, const uint32_t version);

        // If the Open, with a request was made, this method waits for the requested interface.
        template <typename INTERFACE>
        inline INTERFACE* WaitForCompletion(const uint32_t waitTime)
        {
            INTERFACE* result = nullptr;

            ASSERT(_announceMessage.Parameters().InterfaceId() == INTERFACE::ID);
            ASSERT(_announceMessage.Parameters().Implementation() == 0);
            printf(" WPEFramework::RPC::CommunicatorClient::Acquire()->PID<%d><%d> INTERFACE<%s>WaitForCompletion()\n", getpid(), gettid(), (Core::ClassNameOnly(typeid(INTERFACE).name()).Text()).c_str());
            // Lock event until Dispatch() sets it.
            if (_announceEvent.Lock(waitTime) == Core::ERROR_NONE) {

                Core::instance_id implementation(_announceMessage.Response().Implementation());
                printf(" WPEFramework::RPC::CommunicatorClient::Acquire()->PID<%d><%d> _announceMessage.Response().Implementation<0x%x>\n", getpid(), gettid(), implementation);

                if (implementation) {
                    Core::ProxyType<Core::IPCChannel> baseChannel(*this);

                    ASSERT(baseChannel.IsValid() == true);
                    printf(" WPEFramework::RPC::CommunicatorClient::Acquire()->PID<%d><%d> Administrator::Instance().ProxyInstance()\n", getpid(), gettid());
                    Administrator::Instance().ProxyInstance(baseChannel, implementation, true, result);
                }
            }

            return (result);
        }
        inline bool WaitForCompletion(const uint32_t waitTime)
        {
            // Lock event until Dispatch() sets it.
            return (_announceEvent.Lock(waitTime) == Core::ERROR_NONE);
        }
        virtual void Dispatch(Core::IIPC& element);

    protected:
        void StateChange() override;

    private:
        Core::ProxyObject<RPC::AnnounceMessage> _announceMessage;
        Core::Event _announceEvent;
        uint32_t _connectionId;
    };
}
}

#endif // __COM_PROCESSLAUNCH_H
