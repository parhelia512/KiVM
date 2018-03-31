//
// Created by kiva on 2018/2/25.
//
#pragma once

#include <unordered_map>
#include <kivm/kivm.h>
#include <kivm/classfile/classFile.h>
#include <kivm/classLoader.h>
#include <kivm/oop/oopfwd.h>

namespace kivm {
    enum ClassType {
        INSTANCE_CLASS,
        OBJECT_ARRAY_CLASS,
        TYPE_ARRAY_CLASS,
    };

    enum ClassState {
        ALLOCATED,
        LOADED,
        LINKED,
        BEING_INITIALIZED,
        FULLY_INITIALIZED,
        INITIALIZATION_ERROR,
    };

    class Klass {
    private:
        ClassState _state;
        u2 _access_flag;

    protected:
        String _name;
        ClassType _type;

        mirrorOop _java_mirror;
        Klass *_super_class;

    public:
        mirrorOop getJavaMirror() {
            return _java_mirror;
        }

        void setJavaMirror(mirrorOop java_mirror) {
            this->_java_mirror = java_mirror;
        }

        ClassState getClassState() const {
            return _state;
        }

        void setClassState(ClassState _state) {
            this->_state = _state;
        }

        u2 getAccessFlag() const {
            return _access_flag;
        }

        void setAccessFlag(u2 _access_flag) {
            Klass::_access_flag = _access_flag;
        }

        const String &getName() const {
            return _name;
        }

        void setName(const String &_name) {
            this->_name = _name;
        }

        ClassType getClassType() const {
            return _type;
        }

        void setClassType(ClassType _type) {
            this->_type = _type;
        }

        Klass *getSuperClass() const {
            return _super_class;
        }

        void setSuperClass(Klass *_super_class) {
            this->_super_class = _super_class;
        }

        bool isPublic() const {
            return (getAccessFlag() & ACC_PUBLIC) == ACC_PUBLIC;
        }

        bool isPrivate() const {
            return (getAccessFlag() & ACC_PRIVATE) == ACC_PRIVATE;
        }

        bool isProtected() const {
            return (getAccessFlag() & ACC_PROTECTED) == ACC_PROTECTED;
        }

        bool isFinal() const {
            return (getAccessFlag() & ACC_FINAL) == ACC_FINAL;
        }

        bool isStatic() {
            return (getAccessFlag() & ACC_STATIC) == ACC_STATIC;
        }

        bool isAbstract() {
            return (getAccessFlag() & ACC_ABSTRACT) == ACC_ABSTRACT;
        }

    public:
        Klass();

        virtual ~Klass() = default;

        virtual void linkAndInit() = 0;
    };
}

