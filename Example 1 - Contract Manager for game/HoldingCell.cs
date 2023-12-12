using System.Collections;
using System.Collections.Generic;
using UnityEngine;
namespace NPCBehaviour.Areas
{
    // This class is used to keep track of the number of people in a holding cell and the capacity of the holding cell
    public class HoldingCell : MonoBehaviour
    {
        public int occupant_capacity;
        public int assigned_occupants;
        public void Start()
        {
            assigned_occupants = 0;
        }
        public int getrequiredsubjects()
        {
            return occupant_capacity-assigned_occupants;
        }
        public void assignPerson()
        {
            this.assigned_occupants++;
        }
        public void removePerson()
        {
            this.assigned_occupants--;
        }
    }
}