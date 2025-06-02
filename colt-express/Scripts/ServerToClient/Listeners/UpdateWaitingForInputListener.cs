﻿using GameUnitSpace;
using Newtonsoft.Json.Linq;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UpdateWaitingForInputListener : UIEventListenable
{

    public override void updateElement(string data)
    {
        /* PRE: data 
        *
            {
                eventName = "updateWaitingForInput",
                currentPlayer = character,
                waitingForInput = bool,
                canDraw = bool
            };
        */
        JObject o = JObject.Parse(data);
        Character player = o.SelectToken("currentPlayer").ToObject<Character>();
        bool waitingForInput = o.SelectToken("waitingForInput").ToObject<bool>();

        if (NamedClient.c == player)
        {
            //If SCHEMIN, unlock the turn menu
            if (GameUIManager.gameUIManagerInstance.gameStatus == GameStatus.Schemin)
            {
                if (waitingForInput) { 
                    GameUIManager.gameUIManagerInstance.toggleTurnMenu(true);
                    Debug.Log("[UpdateWaitingForInputListener] SCHEMING, TRUE: Turn menu visible for player " + player + ".");
                }
                else
                {
                    GameUIManager.gameUIManagerInstance.toggleTurnMenu(false);
                    GameUIManager.gameUIManagerInstance.lockHand();
                    Debug.Log("[UpdateWaitingForInputListener] SCHEMIN, FALSE: Turn menu hidden for player and hand locked for " + player + ".");
                }

            } else if (GameUIManager.gameUIManagerInstance.gameStatus == GameStatus.Stealin)
            //If STEALIN, unlock the board
            {
                if (waitingForInput)
                {

                    GameUIManager.gameUIManagerInstance.unlockBoard();
                    Debug.Log("[UpdateWaitingForInputListener] STEALIN, TRUE: Board unlocked.");
                }
                else
                {
                    GameUIManager.gameUIManagerInstance.lockBoard();
                    Debug.Log("[UpdateWaitingForInputListener] STEALIN, FALSE: Board locked.");
                }
            } else if (GameUIManager.gameUIManagerInstance.gameStatus == GameStatus.HorseAttack)
            {
                if (waitingForInput)
                {

                    GameUIManager.gameUIManagerInstance.toggleHorseAttackMenu(true);
                    Debug.Log("[UpdateWaitingForInputListener] HORSEATTACK, TRUE: Menu unlocked.");
                }
                else
                {
                    GameUIManager.gameUIManagerInstance.toggleHorseAttackMenu(false);
                    Debug.Log("[UpdateWaitingForInputListener] HORSEATTACK, FALSE: Menu locked.");
                }
            }
            else if (GameUIManager.gameUIManagerInstance.gameStatus == GameStatus.FinalizingCard)
            {
                if (waitingForInput)
                {

                    GameUIManager.gameUIManagerInstance.toggleKeepMenu(true);
                    Debug.Log("[UpdateWaitingForInputListener] HORSEATTACK, TRUE: Menu unlocked.");
                }
                else
                {
                    GameUIManager.gameUIManagerInstance.toggleKeepMenu(false);
                    Debug.Log("[UpdateWaitingForInputListener] HORSEATTACK, FALSE: Menu locked.");
                }
            }
            //If HORSE ATTACK, display horse attack menu
        }
        
    }
}