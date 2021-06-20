#pragma once
#include <unordered_map>
#include <memory>
#include <SFML/Network.hpp>
#include <NetworkObject.h>
#include <MessegesStructs.h>


class Server : public NetworkObject{
public:
    Server();
    virtual ~Server();
    bool launch();
    virtual bool handleRequests(int = 1)override;
    virtual void updateLoc(const MemberInfo&)override;
    virtual void setName(const char name[PLAYER_NAME_LEN], int index = -1)override;
    virtual void sendStaticCollision(int)override;
    virtual void addProjectile(const AddProjectileMessage&)override;
    void startGame();

    //test
    void sendNewInfo(const std::vector<MovingObjInfo>& vec);

    
private:
    //state
    bool m_launched;
    bool m_requiting;

    virtual void notifyClosing()override;
    void notifyCloser(int);
    void registerPlayer();
    void handleNetworkMessege();
    void updateStaticObjState(const StaticObjInfo& info);
    void updateAboutNewMember(const AddMember&);
    int countServersInPort();
    bool renameMember();
    
};