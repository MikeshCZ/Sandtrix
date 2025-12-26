#pragma once
#include "raylib.h"

/**
 * Úvodní animace ve stylu Raylib loga.
 * Zobrazuje animované logo při startu hry s postupným vykreslováním
 * obdélníků a fade in/out efekty textu.
 */
class Intro
{
public:
  /**
   * Konstruktor - inicializuje úvodní animaci.
   * @param title Název hry k zobrazení
   * @param windowWidth Šířka okna pro centrování
   * @param windowHeight Výška okna pro centrování
   */
  Intro (const char *title, int windowWidth, int windowHeight);

  /**
   * Aktualizuje stav animace (postupné vykreslování rámečků).
   */
  void UpdateLogoScreen ();

  /**
   * Vykreslí aktuální frame úvodní animace.
   */
  void DrawLogoScreen ();

  /**
   * Zjistí, zda animace skončila.
   * @return true pokud je animace dokončena
   */
  bool IsIntroFininsh ();

private:
  const char *title;           // Název hry
  int framesCounter{};         // Počítadlo framů pro timing
  bool finishScreen{};         // Příznak konce animace
  int logoPositionX{};         // X pozice loga
  int logoPositionY{};         // Y pozice loga
  int lettersCount{};          // Počet vykreslených písmen titulku
  int topSideRecWidth{};       // Šířka horního obdélníku
  int leftSideRecHeight{};     // Výška levého obdélníku
  int bottomSideRecWidth{};    // Šířka spodního obdélníku
  int rightSideRecHeight{};    // Výška pravého obdélníku
  int state{};                 // Aktuální stav animace
  float alpha{};               // Průhlednost pro fade efekt
  float alphaTitle{};          // Průhlednost titulku
  float alphaPowered{};        // Průhlednost "Powered by" textu
};
