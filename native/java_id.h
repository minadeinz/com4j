#pragma once
#include "op.h"
//
// helper classes to cache jclass/jmethodID/jfieldID
//

// obtains the global jclass objects
class JClassID {
private:
	jclass clazz;
	const char* const name;	// class name
	
	// all JClassID instances are linked by a chain
	JClassID* next;

	static JClassID* init;

	void setup( JNIEnv* env ) {
		_ASSERT(clazz==NULL);
		clazz = static_cast<jclass>(env->NewGlobalRef(env->FindClass(name)));
		_ASSERT(clazz!=NULL);
	}

public:
	JClassID( const char* _name ) : name(_name), next(init) {
		next = init;
		init = this;
	}

	operator jclass () const {
		_ASSERT(clazz!=NULL);
		return clazz;
	}

	// called once when the system is initialized.
	static void runInit( JNIEnv* env ) {
		for( ; init!=NULL; init=init->next )
			init->setup(env);
	}
};


class JMethodID_Base {
protected:
	jmethodID id;
	JClassID& clazz;
	const char* name;
	const char* sig;

	virtual void setup( JNIEnv* env ) =0;

	// all JClassID instances are linked by a chain
	JMethodID_Base* next;

	static JMethodID_Base* init;

public:
	JMethodID_Base( JClassID& _clazz, const char* _name, const char* _sig ) : clazz(_clazz) {
		name = _name;
		sig = _sig;

		next = init;
		init = this;
	}

	operator jmethodID () const {
		return id;
	}

	JClassID& getClazz() { return clazz; }

	// called once when the system is initialized.
	static void runInit( JNIEnv* env ) {
		for( ; init!=NULL; init=init->next )
			init->setup(env);
	}
};

template < class JavaReturnType >
class JMethodID : public JMethodID_Base {
public:
	JMethodID( JClassID& _clazz, const char* _name, const char* _sig ) : JMethodID_Base(_clazz,_name,_sig) {};
protected:
	void setup( JNIEnv* env ) {
		id = env->GetMethodID( clazz, name, sig );
	}
public:
	JavaReturnType operator () ( JNIEnv* env ... ) {
        va_list args;
		va_start(args,env);
		JavaReturnType r = op::Op<JavaReturnType>::invokeV(env,clazz,id,args);
		va_end(args);
		return r;
	}
};

class JStaticMethodID : public JMethodID_Base {
public:
	JStaticMethodID( JClassID& _clazz, const char* _name, const char* _sig ) : JMethodID_Base(_clazz,_name,_sig) {};
protected:
	void setup( JNIEnv* env ) {
		id = env->GetStaticMethodID( clazz, name, sig );
	}
};

class JConstructorID : public JMethodID<jobject> {
public:
	JConstructorID( JClassID& _clazz, const char* _sig ) : JMethodID<jobject>(_clazz,"<init>",_sig) {};

	jobject operator () ( JNIEnv* env ... ) {
        va_list args;
		va_start(args,env);
		jobject r = env->NewObjectV(clazz,id,args);
		va_end(args);
		return r;
	}
};



extern JClassID javaLangNumber;
extern JMethodID<jbyte> javaLangNumber_byteValue;
extern JMethodID<jshort> javaLangNumber_shortValue;
extern JMethodID<jint> javaLangNumber_intValue;
extern JMethodID<jlong> javaLangNumber_longValue;
extern JMethodID<jfloat> javaLangNumber_floatValue;
extern JMethodID<jdouble> javaLangNumber_doubleValue;

extern JClassID javaLangInteger;
extern JConstructorID javaLangInteger_new;
extern JStaticMethodID javaLangInteger_valueOf;

extern JClassID javaLangShort;
extern JStaticMethodID javaLangShort_valueOf;

extern JClassID javaLangLong;
extern JStaticMethodID javaLangLong_valueOf;

extern JClassID javaLangFloat;
extern JStaticMethodID javaLangFloat_valueOf;

extern JClassID javaLangDouble;
extern JStaticMethodID javaLangDouble_valueOf;

extern JClassID javaLangBoolean;
extern JMethodID<jboolean> javaLangBoolean_booleanValue;
extern JStaticMethodID javaLangBoolean_valueOf;

extern JClassID javaLangString;

extern JClassID com4j_COM4J;
extern JMethodID<jint> com4j_COM4J_getPtr;

// reference to org.kohsuke.com4j.comexception
extern JClassID comexception;
extern JConstructorID comexception_new;
extern JClassID com4j_Holder;
extern jfieldID com4j_Holder_value;

extern JClassID com4jWrapper;
extern JConstructorID com4jWrapper_new;