## AllJoyn���弴��
�����Ŀ������AllJoyn����һ�����弴�õ�С���ԣ�����󲿷���ʦ����ģ��������£�

* �������ͻ��˶��ڷ��������֪�ͻ��˷���������Ϣ
* �ͻ����յ�����󣬾ͷ���������Ϣ
* �����յ�������Ϣ�󣬴�֮��mongodb���ݿ⣻���о�������ݣ�Ҳ��ģ����yeelinkƽ̨post����

�����õ����ⲿģ��϶࣬�������ý��鷳���ֽ�������

* Ϊͨ����������ȶ������¼�������������
    * ALLJOYN_DIR(�汾:14.02)������ٷ���SDK����Ŀ¼����bin,inc,lib��Ŀ¼
    * BOOST_DIR(�汾:1.53)����Ŀ¼����boostͷ�ļ��к�lib��Ŀ¼�������Լ����룬�̳̿ɲο�����
    
	����mongo-cxx�Ŀⲻ̫���ң��ֲ�̫�����ԾͰѿ��ͷ�ļ�ֱ�ӷŵ�����ĿĿ¼mongodb�У�
	tinyxml��Ҳ��С��ֱ�ӷ���
* ��Ӹ�ģ��Ŀ�Ŀ¼·����ͷ�ļ�Ŀ¼
* ����alljoyn������Ҫ��Ŀ���
    * alljoyn.lib
ajrouter.lib
BundledRouter.obj
ws2_32.lib
crypt32.lib
iphlpapi.lib
Secur32.lib
Bcrypt.lib
Ncrypt.lib
	
    * tinyxml��
	
	* ����mongoclient���boost�⣬�����Զ�ʶ����ƣ����Բ�����Ӿ����ļ�

* ע���ڹ������Ե�Ԥ����������ӷ���QCC_OS_GROUP_WINDOWS��UNICODE����������������⣺
	\inc\qcc\platform.h(36): fatal error C1004: ����������ļ�β

	����������-c/c++-����ѡ��-����ѡ�������룺/vmm /vmg %(AdditionalOptions)����������ʱ���ִ���The value of ESP not properly saved...
	
* ����boost��mongoclient���Ƕ�̬���ӣ��������г���ǰҪ��֤���ҵ���ȷ��dll