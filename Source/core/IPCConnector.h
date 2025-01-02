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

#ifndef __IPCCONNECTOR_H_
#define __IPCCONNECTOR_H_

#include "Factory.h"
#include "IAction.h"
#include "Link.h"
#include "Module.h"
#include "Portability.h"
#include "SocketPort.h"
#include "TypeTraits.h"

namespace WPEFramework {

namespace Core {

    class IPCChannel;

    struct IMessage {
    public:
        typedef IMessage BaseElement;
        typedef uint32_t Identifier;

        class Serializer {
        public:
            Serializer(const Serializer&) = delete;
            Serializer& operator=(const Serializer&) = delete;

            Serializer()
                : _length(0)
                , _offset(0)
                , _current(nullptr)
            {
            }
            virtual ~Serializer()
            {
                ASSERT(_current == nullptr);
            }

        public:
            bool Submit(const IMessage& element)
            {

                ASSERT(_current == nullptr);
                printf("WPEFramework::Core::IMessage::Serializer::Submit()->PID<%d><%d> \n", getpid(), gettid());
                // TODO: Make sure it is thread safe. The _current needs to
                // be written as the last parameter so in case the
                // serialize gets triggered due to another read/write cycle,
                // Serialize will not start processing until the current (and
                // thius all other parameters) are set correctly.
                _length = element.Length();
                _offset = 0;
                _current = &element;

                ASSERT(_length <= 0x1FFFFFFF);
                printf("WPEFramework::Core::IMessage::Serializer::Submit()->PID<%d><%d> _length<%d>\n", getpid(),gettid(), _length);
                return (true);
            }

            // The Serialize and Deserialize methods allow the content to be serialized/deserialized.
            uint16_t Serialize(uint8_t stream[], const uint16_t maxLength)
            {
                uint16_t result = 0;
                printf("WPEFramework::Core::IMessage::Serializer::Serialize()->PID<%d>TID<%d> maxLength<%d>_offset<%d>\n", getpid(), gettid(),maxLength, _offset);
                while ((_current != nullptr) && (result < maxLength)) {
                    if (_offset < 4) {
                        uint32_t length = _length + CommandSize();

                        printf("WPEFramework::Core::IMessage::Serializer::Serialize()->PID<%d>TID<%d> length<%d> _length<%d>\n", getpid(), gettid(),length, _length);
                        // Write the length. Continue as long as the top bt is active..
                        while ((_offset < 4) && (result < maxLength)) {
                            uint32_t value = length >> (7 * _offset);
                            stream[result] = ((value & 0x7F) | (value >= 0x80 ? 0x80 : 0x00));
                            printf("WPEFramework::Core::IMessage::Serializer::Serialize()->PID<%d>TID<%d> stream[result(%d)]=%x \n", getpid(),  gettid(), result, stream[result]);
                            result++;

                            if (value >= 0x80) {
                                _offset++;
                            } else {
                                _offset = 4;
                            }
                        }
                    }

                    // Write the command, Same structure as length..
                    while ((_offset < 8) && (result < maxLength)) {
                        uint32_t value = _current->Label() >> (7 * (_offset - 4));
                        stream[result] = ((value & 0x7F) | (value >= 0x80 ? 0x80 : 0x00));
                        printf("WPEFramework::Core::IMessage::Serializer::Serialize()->PID<%d><%d> stream[result(%d)]=%x \n", getpid(), gettid(),result, stream[result]);
                        result++;

                        if (value >= 0x80) {
                            _offset++;
                        } else {
                            _offset = 8;
                        }
                    }

                    if (result < maxLength) {
                        // Write the command, Same structure as length..
                        uint16_t handled = _current->Serialize(&stream[result], maxLength - result, _offset - 8);
                        for(int i=result; i < handled; i++){
                            printf("WPEFramework::Core::IMessage::Serializer::Serialize()->PID<%d><%d> stream[result(%d)]=%x \n", getpid(), gettid(), i, stream[i]);
                        }

                        result += handled;
                        _offset += handled;

                        ASSERT_VERBOSE((_offset - 8) <= _length, "%d <= %d", (_offset - 8), _length);

                        if ((_offset - 8) == _length) {
                            const IMessage* ready = _current;
                            _current = nullptr;
                            printf("WPEFramework::Core::IMessage::Serializer::Serialize()->PID<%d><%d> Serialized() \n", getpid(), gettid());
                            // we are done, send out that we copied it all
                            Serialized(*ready);
                        }
                    }
                }

                return (result);
            }
            virtual void Serialized(const IMessage& element) = 0;

        private:
            uint32_t CommandSize() const
            {
                return (_current->Label() > 0x1FFFFF ? 4 : (_current->Label() > 0xCFFF ? 3 : (_current->Label() > 0x7F ? 2 : 1)));
            }

        private:
            uint32_t _length;
            uint32_t _offset;
            const IMessage* _current;
        };

        class Deserializer {
        public:
            Deserializer(const Deserializer&) = delete;
            Deserializer& operator=(const Deserializer&) = delete;

            Deserializer()
                : _length(0)
                , _offset(0)
                , _label(0)
                , _current(nullptr)
            {
            }
            virtual ~Deserializer() = default;

        public:
            virtual void Deserialized(IMessage& element) = 0;
            virtual IMessage* Element(const uint32_t& label) = 0;

            uint16_t Deserialize(const uint8_t stream[], const uint16_t maxLength)
            {
                uint16_t result = 0;
                printf("WPEFramework::Core::IMessage::Deserializer::Deserialize()->PID<%d><%d>maxLength<%d> \n", getpid(), gettid(), maxLength);
                while (result < maxLength) {
					if ((_current == nullptr) && (_offset < 8)) {
                        // We have nothing, start by getting the length/command
                        while ((_offset < 4) && (result < maxLength)) {
                            _length |= ((stream[result] & (_offset == 3 ? 0xFF : 0x7F)) << (7 * _offset));

                            if ((stream[result++] & 0x80) != 0) {
                                _offset++;
                            } else {
                                _offset = 4;
                            }
                        }

                        while ((_offset < 8) && (result < maxLength)) {
                            _label |= ((stream[result] & (_offset == 7 ? 0xFF : 0x7F)) << (7 * (_offset - 4)));
                            _length--;

                            if ((stream[result++] & 0x80) != 0) {
                                _offset++;
                            } else {
                                _offset = 8;
                            }
                        }
                        printf("WPEFramework::Core::IMessage::Deserializer::Deserialize()->PID<%d><%d> _length<%d> _label<%d>\n", getpid(), gettid(), _length, _label);
                        if (_offset == 8) {
                            _current = Element(_label);
                            printf("WPEFramework::Core::IMessage::Deserializer::Deserialize()->PID<%d><%d> _length<%d> _label<%d>\n", getpid(), gettid(),_length, _label);
                            _label = 0;
                        }
                    }

                    if (_offset >= 8) {

                        ASSERT((_offset - 8) <= _length);

                        if ((_offset - 8) < _length) {

                            // There could be multiple packages in this frame, do not read/handle more than what fits in the frame.
                            uint32_t tmp = _length - (_offset - 8);
                            uint16_t handled(static_cast<uint32_t>(maxLength - result) > tmp ? static_cast<uint16_t>(tmp) : (maxLength - result));

                            if (_current != nullptr) {
                                handled = _current->Deserialize(&stream[result], handled, _offset - 8);
                            }
                            for(int i=result; i < handled; i++){
                                printf("WPEFramework::Core::IMessage::Deserializer::Deserialize()->PID<%d><%d> stream[result(%d)]=%x \n", getpid(), gettid(), i, stream[i]);
                            }
                            _offset += handled;
                            result += handled;
                        }

                        ASSERT((_offset - 8) <= _length);

                        if ((_offset - 8) == _length) {
                            if (_current != nullptr) {
                                IMessage* ready = _current;
                                _current = nullptr;
                                printf("WPEFramework::Core::IMessage::Deserializer::Deserialize()->PID<%d><%d> calling Deserialized() \n", getpid(), gettid());
                                Deserialized(*ready);
                            }
                            _offset = 0;
                            _length = 0;
                        }
                    }
                }
                return (result);
            }

        private:
            uint32_t _length;
            uint32_t _offset;
            uint32_t _label;
            IMessage* _current;
        };

    public:
        virtual ~IMessage() = default;

        virtual uint32_t Label() const = 0;
        virtual uint32_t Length() const = 0;
        virtual uint16_t Serialize(uint8_t[] /* stream*/, const uint16_t /* maxLength */, const uint32_t offset) const = 0;
        virtual uint16_t Deserialize(const uint8_t[] /* stream*/, const uint16_t /* maxLength */, const uint32_t offset) = 0;
    };

    struct EXTERNAL IIPC {
        virtual ~IIPC() = default;

        virtual uint32_t Label() const = 0;
        virtual ProxyType<IMessage> IParameters() = 0;
        virtual ProxyType<IMessage> IResponse() = 0;
    };

    struct EXTERNAL IIPCServer {
        virtual ~IIPCServer() = default;

        virtual void Procedure(IPCChannel& source, Core::ProxyType<IIPC>& message) = 0;
    };

    template <const uint32_t IDENTIFIER, typename PARAMETERS, typename RESPONSE>
    class IPCMessageType : public IIPC, public IReferenceCounted {
    private:
        using CompoundClass = IPCMessageType<IDENTIFIER, PARAMETERS, RESPONSE>;

        template <typename PACKAGE, const uint32_t REALIDENTIFIER>
        class RawSerializedType : public Core::IMessage, public Core::IReferenceCounted {
        public:
            using ThisClass = RawSerializedType<PACKAGE, REALIDENTIFIER>;

            RawSerializedType() = delete;
            RawSerializedType(const RawSerializedType<PACKAGE, REALIDENTIFIER>& copy) = delete;
            RawSerializedType<PACKAGE, REALIDENTIFIER>& operator=(const RawSerializedType<PACKAGE, REALIDENTIFIER>& copy) = delete;

            RawSerializedType(CompoundClass& parent)
                : _parent(parent)
                , _package()
            {
            }
            RawSerializedType(CompoundClass& parent, const PACKAGE& package)
                : _parent(parent)
                , _package(package)
            {
            }
            ~RawSerializedType() override = default;

        public:
            uint32_t AddRef() const override {
                return (_parent.AddRef());
            }
            uint32_t Release() const override {
                printf("WPEFramework::Core::IPCMessageType<...>::RawSerializedType<...>::ThisClass::Release()->PID<%d><%d> \n", getpid(), gettid());
                return(_parent.Release());
            }
            void Clear()
            {
                __Clear();
            }
            PACKAGE& Package()
            {
                return (_package);
            }
            uint32_t Label() const override
            {
                return (REALIDENTIFIER);
            }
            uint32_t Length() const override
            {
                return (_Length());
            }
            uint16_t Serialize(uint8_t stream[], const uint16_t maxLength, const uint32_t offset) const override
            {
                return (_Serialize(stream, maxLength, offset));
            }
            uint16_t Deserialize(const uint8_t stream[], const uint16_t maxLength, const uint32_t offset) override
            {
                printf("WPEFramework::Core::IPCMessageType<...>::RawSerializedType<...>::ThisClass::Deserialize()->PID<%d><%d> \n", getpid(), gettid());
                printf("WPEFramework::Core::IPCMessageType<...>::RawSerializedType<...>::ThisClass::Deserialize()->PID<%d><%d> \n", getpid(), gettid());
                return _Deserialize(stream, maxLength, offset);
            }

        private:
            // -----------------------------------------------------
            // Check for Clear method on Object
            // -----------------------------------------------------
            IS_MEMBER_AVAILABLE(Clear, hasClear);

            template <typename SUBJECT=PACKAGE>
            typename Core::TypeTraits::enable_if<hasClear<SUBJECT, void>::value, void>::type
            __Clear()
            {
                _package.Clear();
            }

            template <typename SUBJECT=PACKAGE>
            typename Core::TypeTraits::enable_if<!hasClear<SUBJECT, void>::value, void>::type
            __Clear()
            {
            }

            // -----------------------------------------------------
            // Search for custom handling, Compile time !!!
            // -----------------------------------------------------
            IS_MEMBER_AVAILABLE(Length, hasLength);

            template <typename SUBJECT=PACKAGE>
            typename Core::TypeTraits::enable_if<hasLength<const SUBJECT, uint32_t>::value, uint32_t>::type
            _Length() const
            {
                return (_package.Length());
            }

            template <typename SUBJECT=PACKAGE>
            typename Core::TypeTraits::enable_if<!hasLength<const SUBJECT, uint32_t>::value, uint32_t>::type
            _Length() const
            {
                return (sizeof(PACKAGE));
            }

            IS_MEMBER_AVAILABLE(Serialize, hasSerialize);

            template <typename SUBJECT= PACKAGE>
            typename Core::TypeTraits::enable_if<hasSerialize<const SUBJECT, uint16_t, uint8_t[], const uint16_t, const uint32_t>::value, uint16_t>::type
            _Serialize(uint8_t stream[], const uint16_t maxLength, const uint32_t offset) const
            {
                printf("WPEFramework::Core::IPCMessageType<...>::RawSerializedType<...>::ThisClass::_Serialize()->PID<%d>TID<%d> calling _package.Serialize() \n", getpid(), gettid());
                return (_package.Serialize(stream, maxLength, offset));
            }

            template <typename SUBJECT= PACKAGE>
            typename Core::TypeTraits::enable_if<!hasSerialize<const SUBJECT, uint16_t, uint8_t[], const uint16_t, const uint32_t>::value, uint16_t>::type
            _Serialize(uint8_t stream[], const uint16_t maxLength, const uint32_t offset) const
            {
                uint16_t result = 0;
                uint32_t packageLength = _Length();
                printf("WPEFramework::Core::IPCMessageType<...>::RawSerializedType<...>::ThisClass::_Serialize()->PID<%d>TID<%d> calling memcpy() \n", getpid(), gettid());
                if (offset < packageLength) {
                    result = ((maxLength > (packageLength - offset)) ? (packageLength - offset) : maxLength);
                    ::memcpy(stream, &(reinterpret_cast<const uint8_t*>(&_package)[offset]), result);
                }
                return (result);
            }

            IS_MEMBER_AVAILABLE(Deserialize, hasDeserialize);

            template <typename SUBJECT=PACKAGE>
            typename Core::TypeTraits::enable_if<hasDeserialize<SUBJECT, uint16_t, const uint8_t[], const uint16_t, const uint32_t>::value, uint16_t>::type
            _Deserialize(const uint8_t stream[], const uint16_t maxLength, const uint32_t offset)
            {
                printf("WPEFramework::Core::IPCMessageType<...>::RawSerializedType<...>::ThisClass::_Deserialize()->PID<%d><%d> \n", getpid(), gettid());
                return (_package.Deserialize(stream, maxLength, offset));
            }

            template <typename SUBJECT=PACKAGE>
            typename Core::TypeTraits::enable_if<!hasDeserialize<SUBJECT, uint16_t, const uint8_t[], const uint16_t, const uint32_t>::value, uint16_t>::type
            _Deserialize(const uint8_t stream[], const uint16_t maxLength, const uint32_t offset)
            {
                uint16_t result = 0;

                if (offset < sizeof(SUBJECT)) {
                    result = (maxLength > static_cast<uint16_t>(sizeof(SUBJECT) - offset) ? static_cast<uint16_t>(sizeof(SUBJECT) - offset) : maxLength);
                    ::memcpy(&(reinterpret_cast<uint8_t*>(&_package)[offset]), stream, result);
                }
                printf("WPEFramework::Core::IPCMessageType<...>::RawSerializedType<...>::ThisClass::_Deserialize(memcpy)->PID<%d><%d> result<%d>\n", getpid(), gettid(),result);
                return (result);
            }

        private:
            CompoundClass& _parent;
            PACKAGE _package;
        };

    public:
        using ParameterType = RawSerializedType < PARAMETERS, (IDENTIFIER << 1) >;
        using ResponseType = RawSerializedType < RESPONSE, ((IDENTIFIER << 1) | 0x1) >;

        IPCMessageType(const IPCMessageType<IDENTIFIER, PARAMETERS, RESPONSE>&) = delete;
        IPCMessageType<IDENTIFIER, PARAMETERS, RESPONSE>& operator=(const IPCMessageType<IDENTIFIER, PARAMETERS, RESPONSE>&) = delete;

PUSH_WARNING(DISABLE_WARNING_THIS_IN_MEMBER_INITIALIZER_LIST)
        IPCMessageType()
            : _parameters(*this)
            , _response(*this)
        {
        }
        IPCMessageType(const PARAMETERS& info)
            : _parameters(*this, info)
            , _response(*this)
        {
        }
POP_WARNING()

        ~IPCMessageType() override {
        }

    public:
        static uint32_t Id()
        {
            return (IDENTIFIER);
        }
        void Clear()
        {
            _parameters.Clear();
            _response.Clear();
        }
        PARAMETERS& Parameters()
        {
            return (_parameters.Package());
        }
        RESPONSE& Response()
        {
            return (_response.Package());
        }
        virtual uint32_t Label() const
        {
            return (IDENTIFIER);
        }
        virtual ProxyType<IMessage> IParameters()
        {
            return (ProxyType<IMessage>(_parameters, _parameters));
        }
        virtual ProxyType<IMessage> IResponse()
        {
            return (ProxyType<IMessage>(_response, _response));
        }

    private:
        ParameterType _parameters;
        ResponseType _response;
    };

    class EXTERNAL IPCChannel {
    public:
        class EXTERNAL IPCFactory {
        private:
            friend IPCChannel;

            IPCFactory()
                : _lock()
                , _inbound()
                , _outbound()
                , _callback(nullptr)
                , _factory()
                , _handlers()
            {
            }
            void Factory(Core::ProxyType<FactoryType<IIPC, uint32_t>>& factory)
            {
                ASSERT((_factory.IsValid() == false) && (factory.IsValid() == true));
                printf("WPEFramework::Core::IPCChannel::IPCFactory::Factory()->PID<%d><%d>  _factory = factory \n", getpid(), gettid());
                _factory = factory;
            }

        public:
            IPCFactory(const IPCFactory& copy) = delete;
            IPCFactory& operator=(const IPCFactory&) = delete;

            IPCFactory(Core::ProxyType<FactoryType<IIPC, uint32_t>>& factory)
                : _lock()
                , _inbound()
                , _outbound()
                , _callback(nullptr)
                , _factory(factory)
                , _handlers()
            {
                printf("WPEFramework::Core::IPCChannel::IPCFactory::IPCFactory()->PID<%d><%d> _factory(factory)\n", getpid(), gettid());
                // Only creat the IPCFactory with a valid base factory
                ASSERT(factory.IsValid());
            }
            ~IPCFactory()
            {

                // We expect what you register, to be unregistered before the IPCCHannel is closed !!!
                ASSERT(_handlers.size() == 0);

                std::map<uint32_t, ProxyType<IIPCServer>>::iterator index(_handlers.begin());

                while (index != _handlers.end()) {
                    (*index).second.Release();
                    index++;
                }

                _handlers.clear();
            }

        public:
            void Register(const uint32_t id, const ProxyType<IIPCServer>& handler)
            {
                _lock.Lock();

				ASSERT(handler.IsValid() == true);
                ASSERT(_handlers.find(id) == _handlers.end());

                printf("WPEFramework::Core::IPCChannel::IPCFactory::Register(_handlers)->PID<%d><%d> Handler-Id<%d>\n", getpid(), gettid(), id);
                _handlers.insert(std::pair<uint32_t, ProxyType<IIPCServer>>(id, handler));

                _lock.Unlock();
            }

            void Unregister(const uint32_t id)
            {
                _lock.Lock();

                std::map<uint32_t, ProxyType<IIPCServer>>::iterator index(_handlers.find(id));

                ASSERT(index != _handlers.end());
                printf("WPEFramework::Core::IPCChannel::IPCFactory::Unregister(_handlers)->PID<%d><%d> Handler-Id<%d>\n", getpid(), gettid(), id);
                if (index != _handlers.end()) {
                    _handlers.erase(index);
                }

                _lock.Unlock();
            }

            bool InProgress() const
            {
                printf("WPEFramework::Core::IPCChannel::IPCFactory::InProgress()->PID<%d><%d> _outbound.IsValid<%d> \n", getpid(), gettid(), _outbound.IsValid());
                return (_outbound.IsValid());
            }

            ProxyType<IMessage> Element(const uint32_t& identifier)
            {
                ProxyType<IMessage> result;
                uint32_t searchIdentifier(identifier >> 1);

                _lock.Lock();
                printf("WPEFramework::Core::IPCChannel::IPCFactory::Element()->PID<%d><%d> identifier<%d>\n", getpid(), gettid(),identifier);    
                if (identifier & 0x01) {
                    printf("WPEFramework::Core::IPCChannel::IPCFactory::Element()->PID<%d><%d> identifier & 0x01 \n", getpid(), gettid());    
                    if ((_outbound.IsValid() == true) && (_outbound->Label() == searchIdentifier)) {
                        printf("WPEFramework::Core::IPCChannel::IPCFactory::Element()->PID<%d><%d> calling _outbound->IResponse() \n", getpid(), gettid());    
                        result = _outbound->IResponse();
                    } else {
                        TRACE_L1("Unexpected response message for ID [%d].\n", searchIdentifier);
                    }
                } else {
                    ASSERT(_inbound.IsValid() == false);
                    printf("WPEFramework::Core::IPCChannel::IPCFactory::Element()->PID<%d><%d> ! (identifier & 0x01) searchIdentifier<%d> \n", getpid(), gettid(), searchIdentifier);    
                    ProxyType<IIPC> rpcCall(_factory->Element(searchIdentifier));

                    if (rpcCall.IsValid() == true) {
                        _inbound = rpcCall;
                        printf("WPEFramework::Core::IPCChannel::IPCFactory::Element()->PID<%d><%d> _inbound = rpcCall calling rpcCall->IParameters()\n", getpid(), gettid());    
                        result = rpcCall->IParameters();
                    } else {
                        TRACE_L1("No RPC method definition for ID [%d].\n", searchIdentifier);
                    }
                }

                _lock.Unlock();
                printf("WPEFramework::Core::IPCChannel::IPCFactory::Element()->PID<%d><%d> result->Label<%d> result->Length<%d>\n", getpid(), gettid(), result->Label(), result->Length());    
                return (result);
            }

            void Flush()
            {
                _lock.Lock();

                TRACE_L1("Flushing the IPC mechanims. %d", __LINE__);

                _callback = nullptr;

                if (_outbound.IsValid() == true) {
                    _outbound.Release();
                }
                if (_inbound.IsValid() == true) {
                    _inbound.Release();
                }

                _lock.Unlock();
            }

            ProxyType<IIPCServer> ReceivedMessage(const Core::ProxyType<IMessage>& rhs, Core::ProxyType<IIPC>& inbound)
            {
                ProxyType<IIPCServer> procedure;

                _lock.Lock();
                printf("WPEFramework::Core::IPCChannel::IPCFactory::ReceivedMessage()->PID<%d><%d>\n", getpid(), gettid());
                if ((_outbound.IsValid() == true) && (_outbound->IResponse() == rhs)) {

                    ASSERT(_callback != nullptr);
                    printf("WPEFramework::Core::IPCChannel::IPCFactory::ReceivedMessage(_outbound.IsValid() == true && _outbound->IResponse() == rhs)->PID<%d><%d> \n", getpid(), gettid());
                    ProxyType<IIPC> handledObject(_outbound);

                    _outbound.Release();
                    printf("WPEFramework::Core::IPCChannel::IPCFactory::ReceivedMessage(_outbound.IsValid() == true && _outbound->IResponse() == rhs)->PID<%d><%d> calling  _callback->Dispatch(*handledObject) \n", getpid(), gettid());
                    _callback->Dispatch(*handledObject);
                    _callback = nullptr;
                }
                // If this is *NOT* the outbound call, it is inbound and thus it must have been registered
                else if (_inbound.IsValid() == true) {

                    std::map<uint32_t, ProxyType<IIPCServer>>::iterator index(_handlers.find(_inbound->Label()));

					ASSERT(index != _handlers.end());
                    printf("WPEFramework::Core::IPCChannel::IPCFactory::ReceivedMessage(_inbound.IsValid() == true)->PID<%d><%d> calling Iterate and _handlers.end() \n", getpid(), gettid());
                    if (index != _handlers.end()) {
                        procedure = (*index).second;
                        inbound = _inbound;
                    } else {
                        TRACE_L1("No handler defined to handle the incoming frames. [%d]", _inbound->Label());
                    }
                    printf("WPEFramework::Core::IPCChannel::IPCFactory::ReceivedMessage(_inbound.IsValid() == true)->PID<%d><%d> _inbound.Release() \n", getpid(), gettid());
                    _inbound.Release();
                } else {
                    ASSERT(false && "Received something that is neither an inbound nor on outbound!!!");
                }

                _lock.Unlock();
                printf("WPEFramework::Core::IPCChannel::IPCFactory::ReceivedMessage()->PID<%d> return procedure\n", getpid());
                return (procedure);
            }

            void SetOutbound(const Core::ProxyType<IIPC>& outbound, IDispatchType<IIPC>* callback)
            {
                _lock.Lock();

                ASSERT((outbound.IsValid() == true) && (callback != nullptr));
                ASSERT((_outbound.IsValid() == false) && (_callback == nullptr));
                printf("WPEFramework::Core::IPCChannel::IPCFactory::SetOutbound()->PID<%d><%d> _outbound = outbound _callback = callback\n", getpid(), gettid());
                _outbound = outbound;
                _callback = callback;

                _lock.Unlock();
            }

            bool AbortOutbound()
            {
                bool result = false;

                _lock.Lock();
                printf("WPEFramework::Core::IPCChannel::IPCFactory::AbortOutbound()->PID<%d><%d> \n", getpid(), gettid());
                if (_outbound.IsValid() == true) {

                    result = true;

                    if (_callback != nullptr) {
                        _callback->Dispatch(*_outbound);
                        _callback = nullptr;
                    }

                    _outbound.Release();
                } else {
                    ASSERT(_callback == nullptr);
                }

                _lock.Unlock();

                return (result);
            }

        private:
            mutable CriticalSection _lock;
            Core::ProxyType<IIPC> _inbound;
            mutable Core::ProxyType<IIPC> _outbound;
            IDispatchType<IIPC>* _callback;
            Core::ProxyType<FactoryType<IIPC, uint32_t>> _factory;
            std::map<uint32_t, ProxyType<IIPCServer>> _handlers;
        };

    protected:
        IPCChannel()
            : _administration()
            , _customData(nullptr)
        {
        }

        void Factory(Core::ProxyType<FactoryType<IIPC, uint32_t>>& factory)
        {
            ASSERT(_customData == nullptr);
            printf("WPEFramework::Core::IPCChannel::Factory()->PID<%d><%d> calling _administration.Factory(factory)\n", getpid(), gettid());
            _administration.Factory(factory);
        }

    public:
        IPCChannel(const IPCChannel&) = delete;
        IPCChannel& operator=(const IPCChannel&) = delete;

        IPCChannel(Core::ProxyType<FactoryType<IIPC, uint32_t>>& factory)
            : _administration(factory)
            , _customData(nullptr)
        {
        }
        virtual ~IPCChannel() = default;

    public:
        uintptr_t LinkId() const {
            return (reinterpret_cast<uintptr_t>(this));
        }
        void Register(const uint32_t id, const ProxyType<IIPCServer>& handler)
        {
            printf("WPEFramework::Core::IPCChannel::Register()->PID<%d><%d> calling _administration.Register(id, handler)\n", getpid(), gettid());
            _administration.Register(id, handler);
        }

        void Unregister(const uint32_t id)
        {
            printf("WPEFramework::Core::IPCChannel::Unregister()->PID<%d><%d> calling _administration.Unregister(id, handler)\n", getpid(), gettid());
            _administration.Unregister(id);
        }

        void Abort()
        {
            printf("WPEFramework::Core::IPCChannel::Abort()->PID<%d><%d> calling _administration.AbortOutbound(id, handler)\n", getpid(), gettid());
            _administration.AbortOutbound();
        }
        template <typename ACTUALELEMENT>
        uint32_t Invoke(const ProxyType<ACTUALELEMENT>& command, IDispatchType<IIPC>* completed)
        {
            printf("WPEFramework::Core::IPCChannel::Invoke(command, complete)->PID<%d><%d> ACTUALELEMENT<%s> calling Execute(Core::ProxyType<IIPC>(command), completed)\n", getpid(), gettid(), (Core::ClassNameOnly(typeid(ACTUALELEMENT).name()).Text()).c_str());
            return (Execute(Core::ProxyType<IIPC>(command), completed));
        }
        template <typename ACTUALELEMENT>
        uint32_t Invoke(const ProxyType<ACTUALELEMENT>& command, const uint32_t waitTime)
        {
            printf("WPEFramework::Core::IPCChannel::Invoke(command, waitTime)->PID<%d><%d> ACTUALELEMENT<%s> calling Execute(Core::ProxyType<IIPC>(command), waitTime)\n", getpid(), gettid(), (Core::ClassNameOnly(typeid(ACTUALELEMENT).name()).Text()).c_str());
            return (Execute(Core::ProxyType<IIPC>(command), waitTime));
        }
        uint32_t Invoke(const ProxyType<Core::IIPC>& command, IDispatchType<IIPC>* completed)
        {
            printf("WPEFramework::Core::IPCChannel::Invoke(command, waitTime)->PID<%d><%d> calling Execute(Core::ProxyType<IIPC>(command), completed)\n", getpid(), gettid());
            return (Execute(command, completed));
        }
        uint32_t Invoke(const ProxyType<Core::IIPC>& command, const uint32_t waitTime)
        {
            printf("WPEFramework::Core::IPCChannel::Invoke(command, waitTime)->PID<%d><%d> calling Execute(Core::ProxyType<IIPC>(command), completed)\n", getpid(), gettid());
            return (Execute(command, waitTime));
        }

        const void* CustomData() const
        {
            return (_customData);
        }
        void CustomData(const void* const data)
        {
            _customData = data;
        }

        virtual uint32_t ReportResponse(Core::ProxyType<IIPC>& inbound) = 0;

    private:
        virtual uint32_t Execute(const ProxyType<IIPC>& command, IDispatchType<IIPC>* completed) = 0;
        virtual uint32_t Execute(const ProxyType<IIPC>& command, const uint32_t waitTime) = 0;

    protected:
        IPCFactory _administration;

    private:
        const void* _customData;
    };

    template <typename ACTUALSOURCE, typename EXTENSION>
    class IPCChannelType : public IPCChannel {
    private:
        class IPCLink : public LinkType<ACTUALSOURCE, IMessage, IMessage, IPCFactory&> {
        private:
            using BaseClass = LinkType<ACTUALSOURCE, IMessage, IMessage, IPCFactory&>;

        public:
            IPCLink() = delete;
            IPCLink(const IPCLink&) = delete;
            IPCLink& operator=(const IPCLink&) = delete;

            template <typename... Args>
            IPCLink(IPCChannelType<ACTUALSOURCE, EXTENSION>* parent, IPCFactory* factory, Args&&... args)
                : LinkType<ACTUALSOURCE, IMessage, IMessage, IPCFactory&>(2, *factory, std::forward<Args>(args)...)
                , _factory(*factory)
                , _parent(*parent)
            {
            }
            ~IPCLink() override = default;

        public:
            bool SendResponse(Core::ProxyType<IIPC>& inbound)
            {
                ASSERT(inbound.IsValid() == true);

                printf("WPEFramework::Core::IPCChannelType<ACTUALSOURCE, EXTENSION>::IPCLink::SendResponse()->PID<%d> calling BaseClass::Submit()\n", getpid());
                // This is an inbound call, Report what we have processed !!!
                return (BaseClass::Submit(inbound->IResponse()));
            }

            // Notification of a INBOUND element received.
            void Received(Core::ProxyType<IMessage>& message) override
            {

                Core::ProxyType<IIPC> inbound;
                printf("WPEFramework::Core::IPCChannelType<ACTUALSOURCE, EXTENSION>::IPCLink::Received()->PID<%d> Label<%d> calling handler()\n", getpid(), message->Label());
                ProxyType<IIPCServer> handler(_factory.ReceivedMessage(message, inbound));

                if (handler.IsValid() == true) {
                    printf("WPEFramework::Core::IPCChannelType<ACTUALSOURCE, EXTENSION>::IPCLink::Received()->PID<%d> calling _parent.CallProcedure(handler, inbound)\n", getpid());
                    _parent.CallProcedure(handler, inbound);
                }
            }

            // Notification of a Response send.
            void Send(const Core::ProxyType<IMessage>& message VARIABLE_IS_NOT_USED) override
            {
                // Oke, nice we send out the info. Nothing to do. It will all be triggered by the Receive..
            }

            // Notification of a channel state change..
            void StateChange() override
            {
                printf("WPEFramework::Core::IPCChannelType<ACTUALSOURCE, EXTENSION>::IPCLink::StateChange()->PID<%d><%d>\n", getpid(),gettid());
                if (_parent.Source().IsOpen() == false) {
                    // Whatever s hapening, Flush what we were doing..
                    _parent.Abort();
                    _factory.Flush();
                }
                printf("WPEFramework::Core::IPCChannelType<ACTUALSOURCE, EXTENSION>::IPCLink::StateChange()->PID<%d><%d> calling _parent.StateChange()\n", getpid(), gettid());
                _parent.StateChange();
            }

        private:
            IPCFactory& _factory;
            IPCChannelType<ACTUALSOURCE, EXTENSION>& _parent;
        };
        class IPCTrigger : public IDispatchType<IIPC> {
        public:
            IPCTrigger() = delete;
            IPCTrigger(const IPCTrigger&) = delete;
            IPCTrigger& operator=(const IPCTrigger&) = delete;

            IPCTrigger(IPCFactory& administration)
                : _administration(administration)
                , _signal(false, true)
            {
            }
            ~IPCTrigger() override = default;

        public:
            uint32_t Wait(const uint32_t waitTime)
            {
                uint32_t result = Core::ERROR_NONE;
                printf("WPEFramework::Core::IPCChannelType<ACTUALSOURCE, EXTENSION>::IPCTrigger::Wait()->PID<%d><%d>waitTime<%d> ACTUALSOURCE<%s>\n", getpid(), gettid(), waitTime, (Core::ClassNameOnly(typeid(ACTUALSOURCE).name()).Text()).c_str());
                // Now we wait for ever, to get a signal that we are done :-)
                if (_signal.Lock(waitTime) != Core::ERROR_NONE) {
                    _administration.AbortOutbound();
                    printf("WPEFramework::Core::IPCChannelType<ACTUALSOURCE, EXTENSION>::IPCTrigger::Wait()->PID<%d><%d> Core::ERROR_TIMEDOUT \n", getpid(), gettid());
                    result = Core::ERROR_TIMEDOUT;
                } else if (_administration.AbortOutbound() == true) {
                    printf("WPEFramework::Core::IPCChannelType<ACTUALSOURCE, EXTENSION>::IPCTrigger::Wait()->PID<%d><%d> Core::ERROR_ASYNC_FAILED \n", getpid(), gettid());
                    result = Core::ERROR_ASYNC_FAILED;
                }
                printf("WPEFramework::Core::IPCChannelType<ACTUALSOURCE, EXTENSION>::IPCTrigger::Wait()->PID<%d><%d> return \n", getpid(), gettid());
                return (result);
            }
            void Dispatch(IIPC& /* element */) override
            {
                printf("WPEFramework::Core::IPCChannelType<ACTUALSOURCE, EXTENSION>::IPCTrigger::Dispatch()->PID<%d><%d> ACTUALSOURCE<%s> calling _signal.SetEvent() \n", getpid(), gettid(), (Core::ClassNameOnly(typeid(ACTUALSOURCE).name()).Text()).c_str());
                _signal.SetEvent();
            }

        private:
            IPCFactory& _administration;
            Event _signal;
        };

    public:
        IPCChannelType(const IPCChannelType<ACTUALSOURCE, EXTENSION>&) = delete;
        IPCChannelType<ACTUALSOURCE, EXTENSION>& operator=(const IPCChannelType<ACTUALSOURCE, EXTENSION>&) = delete;

PUSH_WARNING(DISABLE_WARNING_THIS_IN_MEMBER_INITIALIZER_LIST)
        template <typename... Args>
        IPCChannelType(Args&&... args)
            : IPCChannel()
            , _link(this, &_administration, std::forward<Args>(args)...)
            , _extension(this)
        {
        }
        template <typename... Args>
        IPCChannelType(Core::ProxyType<FactoryType<IIPC, uint32_t>>& factory, Args&&... args)
            : IPCChannel(factory)
            , _link(this, &_administration, std::forward<Args>(args)...)
            , _extension(this)
        {
        }
POP_WARNING()

        ~IPCChannelType() override = default;

    public:
        EXTENSION& Extension()
        {
            return (_extension);
        }
        const EXTENSION& Extension() const
        {
            return (_extension);
        }
        ACTUALSOURCE& Source()
        {
            return (_link.Link());
        }
        const ACTUALSOURCE& Source() const
        {
            return (_link.Link());
        }
        bool InProgress() const
        {
            return (_administration.InProgress());
        }
        uint32_t ReportResponse(Core::ProxyType<IIPC>& inbound) override
        {
            // We got the event, start the invoke, wait for the event to be set again..
            _link.SendResponse(inbound);

            return (Core::ERROR_NONE);
        }
        virtual void StateChange()
        {
            __StateChange();
        }

    private:
        IS_MEMBER_AVAILABLE(StateChange, hasStateChange);

        template <typename T=EXTENSION>
        typename Core::TypeTraits::enable_if<hasStateChange<T, void> ::value, void>::type
        __StateChange()
        {
            _extension.StateChange();
        }


        template <typename T=EXTENSION>
        typename Core::TypeTraits::enable_if<!hasStateChange<T, void> ::value, void>::type
        __StateChange()
        {
        }

        uint32_t Execute(const ProxyType<IIPC>& command, IDispatchType<IIPC>* completed) override
        {
            uint32_t success = Core::ERROR_UNAVAILABLE;

            _serialize.Lock();
            printf("WPEFramework::Core::IPCChannelType<...>::Execute(command, complete)->PID<%d><%d> \n", getpid(), gettid());
            if (_administration.InProgress() == true) {
                success = Core::ERROR_INPROGRESS;
            } else if (_link.IsOpen() == true) {
                // We need to accept a CONST object to avoid an additional object creation
                // proxy casted objects.
                _administration.SetOutbound(command, completed);

                printf("WPEFramework::Core::IPCChannelType<...>::Execute(command, complete)->PID<%d><%d> label<%d>\n", getpid(),gettid(),command->Label());
                printf("WPEFramework::Core::IPCChannelType<...>::Execute(command, complete)->PID<%d><%d> _calling link.Submit()\n", getpid(), gettid());
                // Send out the
                _link.Submit(command->IParameters());

                success = Core::ERROR_NONE;
            }

            _serialize.Unlock();

            return (success);
        }
        uint32_t Execute(const ProxyType<IIPC>& command, const uint32_t waitTime) override
        {
            uint32_t success = Core::ERROR_CONNECTION_CLOSED;

            _serialize.Lock();
            printf("WPEFramework::Core::IPCChannelType<...>::Execute(command, waitTime)->PID<%d><%d> \n", getpid(), gettid());

            if (_link.IsOpen() == true) {
                IPCTrigger sink(_administration);

                printf("WPEFramework::Core::IPCChannelType<...>::Execute(command, waitTime)->PID<%d><%d> calling _administration.SetOutbound()\n", getpid(), gettid());
                // We need to accept a CONST object to avoid an additional object creation
                // proxy casted objects.
                _administration.SetOutbound(command, &sink);

                printf("WPEFramework::Core::IPCChannelType<...>::Execute(command, waitTime)->PID<%d><%d> calling _link.Submit(command->IParameters()\n", getpid(), gettid());
                // Send out the
                _link.Submit(command->IParameters());

                printf("WPEFramework::Core::IPCChannelType<...>::Execute(command, waitTime)->PID<%d><%d> calling sink.Wait()\n", getpid(), gettid());
                success = sink.Wait(waitTime);
            }

            _serialize.Unlock();

            return (success);
        }
        void CallProcedure(ProxyType<IIPCServer>& procedure, ProxyType<IIPC>& message)
        {
            printf("WPEFramework::Core::IPCChannelType<...>::CallProcedure()->PID<%d><%d>IIPCServer<%s> IIPC<%s>\n", getpid(), gettid(), (Core::ClassNameOnly(typeid(IIPCServer).name()).Text()).c_str(), (Core::ClassNameOnly(typeid(IIPC).name()).Text()).c_str());
            procedure->Procedure(*this, message);
        }

    private:
        CriticalSection _serialize;
        IPCLink _link;
        EXTENSION _extension;
    };
}
} // namespace Core

#endif // __IPCCONNECTOR_H_
