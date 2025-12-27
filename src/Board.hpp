#pragma once

#include "raylib.h"
#include "Particle.hpp"
#include "ExplosionParticle.hpp"
#include <vector>
#include <set>
#include <unordered_map>

/**
 * Správa herní desky, fyziky částic a výbuchových efektů.
 * Implementuje prostorovou mříž pro efektivní detekci kolizí O(1),
 * gravitaci, BFS algoritmus pro hledání propojených skupin částic
 * a třífázový výbuchový systém (NONE -> ZOOMING -> EXPLODING).
 */
class Board {
public:
    /**
     * Stavy výbuchové animace:
     * - NONE: Žádný výbuch
     * - ZOOMING: Předvýbuchová animace zvětšování částic
     * - EXPLODING: Samotný výbuch s efekty částic
     */
    enum class ExplosionState { NONE, ZOOMING, EXPLODING };

    int width, height;                                              // Rozměry desky v buňkách částic
    std::vector<Particle*> particles;                              // Všechny aktivní částice na desce
    std::vector<ExplosionParticle> explosion_particles;            // Efektové částice výbuchu
    std::vector<std::vector<Particle*>> grid;                      // 2D mříž pro O(1) detekci kolizí
    RenderTexture2D background_texture;                            // Předrenderované pozadí pro výkon

    int shake_amount, shake_duration, dir_index;                   // Parametry třesení obrazovky
    bool grid_dirty;                                               // Příznak pro rebuild mříže

    ExplosionState explosion_state;                                // Aktuální stav výbuchu
    int explosion_timer;                                           // Časovač výbuchové animace
    std::set<Particle*> particles_to_explode;                      // Částice určené k výbuchu
    std::unordered_map<Particle*, float> particle_scale_factors;   // Škálovací faktory pro zoom animaci
    Color explosion_flash_color;                                   // Barva blesku při výbuchu

    /**
     * Konstruktor - inicializuje desku, vytvoří mříž a pozadí.
     */
    Board();

    /**
     * Destruktor - uvolňuje paměť alokovanou pro částice.
     */
    ~Board();

    /**
     * Vytvoří předrenderovanou texturu pozadí pro optimalizaci výkonu.
     */
    void CreateBackground();

    /**
     * Přidá nové částice na desku (z umístěného tetromina).
     * @param new_particles Vektor částic k přidání
     */
    void AddParticles(const std::vector<Particle>& new_particles);

    /**
     * Spustí efekt třesení obrazovky (např. při výbuchu).
     * @param intensity Intenzita třesení (výchozí 10)
     */
    void TriggerShake(int intensity = 10);

    /**
     * Aktualizuje stav třesení obrazovky (snižuje intenzitu).
     */
    void UpdateShake();

    /**
     * Vypočítá aktuální offset pro efekt třesení.
     * @return Vector2 s offsetem pro x a y
     */
    Vector2 GetShakeOffset();

    /**
     * Kontroluje, zda se testované částice nesráží s existującími.
     * Používá prostorovou mříž pro efektivní O(1) detekci.
     * @param test_particles Částice k testování
     * @return true pokud nastane kolize
     */
    bool CheckCollision(const std::vector<Particle>& test_particles);

    /**
     * Přestaví prostorovou mříž na základě aktuálních pozic částic.
     * Volá se po pohybu částic nebo změně jejich pozic.
     */
    void RebuildGrid();

    /**
     * Aplikuje gravitaci na neusazené částice.
     * Částice padají dolů dokud nenarazí na překážku nebo dno.
     */
    void ApplyGravity();

    /**
     * Zkontroluje, zda jsou všechny částice na desce usazené.
     * @return true pokud jsou všechny částice settled, jinak false
     */
    bool AreAllParticlesSettled();

    /**
     * Najde všechny propojené částice stejné barvy pomocí BFS algoritmu.
     * Částice musí být v kontaktu horizontálně nebo vertikálně.
     * @param start Startovní částice pro BFS
     * @return Set všech částic v propojené skupině
     */
    std::set<Particle*> FindConnectedGroup(Particle* start);

    /**
     * Kontroluje všechny částice a hledá skupiny 4+ propojených částic stejné barvy.
     * Pokud najde, označí je k výbuchu.
     * @return Počet nalezených skupin k výbuchu
     */
    int CheckHorizontalConnections();

    /**
     * Aktualizuje předvýbuchovou zoom animaci (ZOOMING stav).
     * Částice se postupně zvětšují před výbuchem.
     */
    void UpdatePreExplosionAnimation();

    /**
     * Aktualizuje výbuchový systém - animace, částice, flash efekt.
     * Spravuje přechody mezi stavy výbuchu.
     */
    void UpdateExplosions();

    /**
     * Vykreslí desku, všechny částice a výbuchové efekty.
     * @param offset_x Horizontální offset na obrazovce
     * @param offset_y Vertikální offset na obrazovce
     */
    void Draw(int offset_x, int offset_y);
};
