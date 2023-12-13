using System.Collections;
using System.Collections.Generic;
using UnityEngine;

//A basic manager interface that all managers should implement this interface is not fully complete yet.
public interface IManager 
{
    public IContract getOpenContract(string key);
    //Todo Add contract
    //Todo Remove contract

}
