using System.Collections;
using System.Collections.Generic;
using UnityEngine;

// An interface for a generic contract, the idea of the contract is to be able to have enough information to either complete the contract or cancel it.
public interface IContract
{
    public string key { get; set; }
    public bool isContractValid { get; set; }
    public IManager Manager { get; set; }
}
