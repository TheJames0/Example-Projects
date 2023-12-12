using NPCBehaviour.Areas;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Diagnostics.Contracts;
using System.Linq;
using UnityEditor;
using UnityEngine;

//A contract struct which is used to store the information of the contract including status and information on the candidates within the contract
public struct TakeToCell_Contract : IContract
{
    public string key { get; set; }
    public bool isContractValid { get; set; }
    public IManager Manager { get; set; }

    public HoldingCell cell;
    public List<IHuman> all_candidates;
    public List<IHuman> remaining_candidates;
    public void initLists()
    {
        all_candidates = new List<IHuman>();
        remaining_candidates = new List<IHuman>();
    }
    public IHuman getRandomCandidate()
    {
        int index = UnityEngine.Random.Range(0, remaining_candidates.Count);
        IHuman retrieved = remaining_candidates[index];
        remaining_candidates.Remove(retrieved);
        return retrieved;
    }
}

//This class manages prison cells and prisoners by assigning the correct number of prisoners to cells. This is managed with contracts because the contract may need to be cancelled at any time.
public class PrisonCellManager : BasicNPCManage, IManager
{
    public List<HoldingCell> all_cells;
    public List<HoldingCell> available_cells;
    public List<IHuman> alldclass;
    public List<IHuman> availabledclass;
    Dictionary<string, TakeToCell_Contract> openContracts;
    protected override void doAwake()
    {

        all_cells = FindObjectsOfType<HoldingCell>().ToList();
        available_cells = all_cells;

        alldclass = new List<IHuman>();
        availabledclass = new List<IHuman>();

        openContracts = new Dictionary<string, TakeToCell_Contract>();
    }

    // Reserve a contract for a cell. This will return a contract which will be used to manage the prisoners and the cell.
    public TakeToCell_Contract Reserve()
    {
        TakeToCell_Contract contract = new TakeToCell_Contract();
        contract.key = Guid.NewGuid().ToString();
        contract.isContractValid = true;
        contract.initLists();

        if(availabledclass.Count == 0 || available_cells.Count == 0) 
        {
            contract.isContractValid = false;
            return contract;
        }
        HoldingCell cell = available_cells.OrderBy(cell => cell.getrequiredsubjects()).FirstOrDefault();
        int accepted_count = cell.getrequiredsubjects();
        accepted_count = Math.Clamp(accepted_count, 0, availabledclass.Count);
        accepted_count = Math.Clamp(accepted_count, 0, 2);
        
        for(int i = 0; i < accepted_count;i++)
        {
            IHuman subject = pickNextCandidate();
            contract.all_candidates.Add(subject);
            contract.remaining_candidates.Add(subject);
            availabledclass.Remove(subject);
            cell.current_subjects += 1;
        }
        if(cell.getrequiredsubjects() == 0)
        {
            available_cells.Remove(cell);
        }
        contract.cell = cell;
        contract.Manager = this;

        openContracts.Add(contract.key, contract);
        return contract;
    }
    // return information on a contract
    public IContract getOpenContract(string key)
    {
        TakeToCell_Contract toReturn;
        if(openContracts.TryGetValue(key, out TakeToCell_Contract contract))
        {
            toReturn = contract;
        }
        else
        {
            toReturn = new TakeToCell_Contract();
            toReturn.isContractValid = false;
        }

        return toReturn;
    }
    //Pick random candidate from all available candidates
    private IHuman pickNextCandidate()
    {
        return availabledclass.ElementAt(0);
    }
    //Returns is a contract is possible
    public bool isContractAvailable()
    {
        if (availabledclass.Count == 0 || available_cells.Count == 0)
        {
            return false;
        }
        else
        { return true; }
    }

    //Event driven function which is called when a new prisoner is added to the game
    protected override void AddCharacter(INPC npc)
    {
        if (npc is not IHuman)
        {
            return;
        }
        IHuman candidate = (IHuman)npc;
        alldclass.Add(candidate);
        availabledclass.Add(candidate);
    }
    //Event driven function which is called when a prisoner is removed from the game
    protected override void RemoveCharacter(INPC npc)
    {
        if (npc is not IHuman)
        {
            return;
        }
            IHuman candidate = (IHuman)npc;
            alldclass.Remove(candidate);
            if (availabledclass.Contains(candidate)) { availabledclass.Remove(candidate); }
            foreach (KeyValuePair<string, TakeToCell_Contract> entry in openContracts)
            {
                foreach (IHuman human in entry.Value.all_candidates)
                {
                    if (human == candidate)
                    {
                        openContracts.Remove(entry.Key);
                        return;
                    }

                }
            }
        }
    
}
