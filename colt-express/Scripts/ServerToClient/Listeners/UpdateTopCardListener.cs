﻿using CardSpace;
using GameUnitSpace;
using Newtonsoft.Json.Linq;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class UpdateTopCardListener : UIEventListenable
{
    private Sprite hiddenCardBack;
    public GameObject playedCards;
    public static int turmoilCardsPlayed = 0;

    public override void updateElement(string data)
    {
        /* PRE: data 
        *
            {
                eventName = action,
                fromPlayer = (Character)args[0],
                card = (ActionKind)args[1]
            };
        */

        JObject o = JObject.Parse(data);
        Character from = o.SelectToken("fromPlayer").ToObject<Character>();
        ActionKind card = o.SelectToken("card").ToObject<ActionKind>();
        GameObject topCard;
        //Account for client adding cards themselves
        if (from != NamedClient.c)
        {
            Debug.Log("[UpdateTopCardListener] Top card not from client added.");
            //Create card under playedCards for nonclient cards
            topCard = GameUIManager.gameUIManagerInstance.createCardObject(from, card, false);
            topCard.SetActive(true);

        } else
        {
            Debug.Log("[UpdateTopCardListener] Top card is already from client.");
            topCard = playedCards.transform.GetChild(playedCards.transform.childCount - 1).gameObject;
        }

        //If topcard was played during a tunnel turn
        if (GameUIManager.gameUIManagerInstance.isTunnelTurn)
        {
            topCard.GetComponent<Image>().sprite = Resources.Load<Sprite>("Sprites/hiddenCard");
            topCard.GetComponent<CardID>().isHidden = true;
        }

        //If topcard was played during a turmoil turn
        if (GameUIManager.gameUIManagerInstance.isTurmoilTurn)
        {
            topCard.GetComponent<Image>().sprite = Resources.Load<Sprite>("Sprites/hiddenCard");
            topCard.GetComponent<CardID>().isHidden = true;
            //Count how many were played, in case someone chose to draw
            turmoilCardsPlayed++;
        }

        bool ghostChoseToHide = o.SelectToken("ghostChoseToHide").ToObject<bool>();
        if (ghostChoseToHide && from == Character.Ghost)
        {
            topCard.GetComponent<Image>().sprite = Resources.Load<Sprite>("Sprites/hiddenCard");
            topCard.GetComponent<CardID>().isHidden = true;
            topCard.GetComponent<CardID>().playedByGhost = true;
        }

        bool photographerHideDisabled = o.SelectToken("photographerHideDisabled").ToObject<bool>();
        if (photographerHideDisabled)
        {
            GameUIManager.gameUIManagerInstance.flipCardObject(from, card, topCard);
            topCard.GetComponent<CardID>().playedByGhost = false;
            topCard.GetComponent<CardID>().isHidden = false;
        }

        //Remove the Draggable component
        Destroy(topCard.GetComponent<Draggable>());

        //If most recent added card brought the total > 7, set the first visible back card invisible
        if (playedCards.transform.childCount >= 8)
        {
            foreach (Transform t in playedCards.transform)
            {
                if (t.gameObject.activeSelf)
                {
                    t.gameObject.SetActive(false);
                    break;
                }
            }
        }

    }
}
