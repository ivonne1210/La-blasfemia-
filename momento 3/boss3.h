#ifndef BOSS3_H
#define BOSS3_H

#include "actor.h"
#include <QVector>
#include <QPixmap>
#include <QGraphicsRectItem>

class Player3;
class BoomerangProjectile;
class MeteorProjectile;

class Boss3 : public Actor
{
public:
    enum AttackType {
        Boomerang,
        JumpAttack,
        MeteorRain
    };

    Boss3(Player3 *player, QGraphicsItem *parent = nullptr);

    void updateEntity(qreal dt) override;

    // Física básica
    void setGroundY(qreal y) { groundY = y; }

    // Llamado por proyectiles cuando golpean al jugador
    void registerPlayerHit(AttackType type);

    // Llamado por el boomerang cuando vuelve al boss
    //void onBoomerangReturned(BoomerangProjectile *b);

    // Para animaciones (idle / cast / salto / daño)
    // luego las llenamos con tus sprites
    void setIdleSprites(const QVector<QPixmap> &frames);
    void setCastSprites(const QVector<QPixmap> &frames);
    void setJumpSprites(const QVector<QPixmap> &frames);

    void setBoomCastSprites(const QVector<QPixmap> &frames);
    void setMeteorCastSprites(const QVector<QPixmap> &frames);
    void onBoomerangReturned(BoomerangProjectile *b);

    int health() const { return m_health; }
    int maxHealth() const { return m_maxHealth; }
    void setHealth(int h)  { m_health = qBound(0, h, m_maxHealth); }
    void applyDamage(int d) { setHealth(m_health - d); }

    // --- Estados para la escena ---
    bool isOnGround()   const { return onGround; }
    bool isJumpAttacking() const { return state == DoJump; }
private:
    // --- Máquina de estados interna ---
    enum State {
        Idle,
        MoveToMeteorPos,
        ChoosingAttack,
        DoBoomerang,
        DoJump,
        DoMeteorRain,
        Recover
    };

    void updatePhysics(qreal dt);
    void updateState(qreal dt);
    void updateAnimation(qreal dt);

    void startNextAttack();
    void startBoomerang();
    void startJumpAttack();
    void startMeteorRain();
    void finishAttack();

    // Helpers
    void applyAnimFrame();

    // Referencias
    Player3 *playerRef = nullptr;

    // Estado general
    State   state = Idle;
    qreal   stateTimer = 0.0;      // tiempo dentro del estado
    qreal   attackCooldown = 1.0;  // tiempo de "respiro" entre ataques

    // Física
    qreal vx = 0.0;
    qreal vy = 0.0;
    qreal gravity   = 1200.0;
    qreal groundY   = 1000.0;   // ajusta al piso del boss
    bool  onGround  = false;

    // Animación
    enum AnimState { AIdle, ACast, AJump, AFall };
    AnimState animState = AIdle;
    int   currentFrame  = 0;
    qreal animTimer     = 0.0;
    qreal frameDuration = 0.10;   // 10 fps

    int facing = -1; // mira a la izquierda por defecto

    QVector<QPixmap> idleFrames;
    QVector<QPixmap> castFrames;
    QVector<QPixmap> jump;
    QVector<QPixmap> boomCastFrames;
    QVector<QPixmap> meteorCastFrames;
    QPixmap boomerangSprite;
    QPixmap meteorSprite;

    QVector<QPixmap>* currentCastFrames = nullptr;
    // --- Boomerang ---
    bool boomerangActive = false;
    BoomerangProjectile *boomerang = nullptr;

    // --- MeteorRain ---
    bool meteorActive = false;
    qreal meteorDuration = 5.0;     // 5 s de lluvia
    qreal meteorSpawnInterval = 0.3;
    qreal meteorSpawnTimer = 0.0;
    qreal meteorTargetX = 450.0;
    qreal meteorTargetY = 600.0;
    QVector<MeteorProjectile*> meteors;

    // --- "Cognitivo" (stats de ataques) ---
    struct AttackStats {
        int used = 0;
        int hits = 0;
    };
    AttackStats statsBoomerang;
    AttackStats statsJump;
    AttackStats statsMeteor;

    // --- Vida del boss ---
    int   m_maxHealth = 300;     // aprox lo que dijiste
    int   m_health    = 300;

    QGraphicsRectItem *hpBg  = nullptr;  // fondo rojo
    QGraphicsRectItem *hpFg  = nullptr;  // barra verde
    qreal hpBarWidth = 140.0;            // ancho en píxeles

    void updateHpBar();                  // helper para actualizar la barra
};

#endif // BOSS3_H
