
using System.Collections;
using System.Collections.Generic;
using System.Net;
using Unity.VisualScripting;
using UnityEngine;

public interface ICullable
{
    Component comp { get; set; }
    public void ToggleOn();
    public void ToggleOff();
}

public class BehaviourCull : MonoBehaviour, ICullable
{
    public Component comp { get; set; }

    public void ToggleOff()
    {
        Behaviour behaviour = (Behaviour)comp;
        behaviour.enabled = false;
    }

    public void ToggleOn()
    {
        Behaviour behaviour = (Behaviour)comp;
        behaviour.enabled = true;
    }
}
public class MeshCull : MonoBehaviour, ICullable
{
    public Component comp { get; set; }

    public void ToggleOff()
    {
        MeshRenderer meshRenderer = (MeshRenderer)comp;
        meshRenderer.enabled = false;
    }

    public void ToggleOn()
    {
        MeshRenderer meshRenderer = (MeshRenderer) comp;
        meshRenderer.enabled = true;
    }
}
public class AudioCull : MonoBehaviour, ICullable
{
    public Component comp { get; set; }
    float time;
    public void ToggleOff()
    {
        AudioSource audioSource = (AudioSource)comp;
        time = audioSource.time;
        audioSource.enabled = false;
    }

    public void ToggleOn()
    {
        AudioSource audioSource = (AudioSource)comp;
        if(!audioSource.enabled)
        {
            audioSource.enabled = true;
            audioSource.Play();
            audioSource.time = time;
        }
    }
}
public class LightCull : MonoBehaviour, ICullable
{
    public Component comp { get; set; }

    public void ToggleOff()
    {
        Light Lightsource = (Light)comp;
        Lightsource.enabled = false;
    }

    public void ToggleOn()
    {
        Light Lightsource = (Light)comp;
        Lightsource.enabled = true;
    }
}

public class ComponentCull : MonoBehaviour
{
    public List<ICullable> comp;

    private void Start()
    {
        comp = new List<ICullable>();
        FindComps();

        
    }
    private void GetComponentRecursive(Transform root, List<ICullable> components)
    {
        Component[] componentarray = root.GetComponents<Component>();
        foreach(Component component in componentarray)
        {
            if (component != null && component != this)
            {
                // Add Icullable to array
                if (typeof(MeshRenderer) == component.GetType())
                {
                    MeshCull meshCull = new()
                    {
                        comp = component
                    };
                    components.Add(meshCull);
                }
                if (typeof(Behaviour) == component.GetType())
                {
                    BehaviourCull behaviourCull = new()
                    {
                        comp = component
                    };
                    components.Add(behaviourCull);
                }
                if (typeof(Light) == component.GetType())
                {
                    LightCull lightCull = new()
                    {
                        comp = component
                    };
                    components.Add(lightCull);
                }
                if (typeof(AudioSource) == component.GetType())
                {
                    AudioCull audioCull = new()
                    {
                        comp = component
                    };
                    components.Add(audioCull);
                }
            }
        }
       

        // Recursively check children
        foreach (Transform child in root)
        {
            GetComponentRecursive(child, components);
        }
    }
    void FindComps()
    {
        GetComponentRecursive(transform, comp);
    }
    public void enableComps()
    {
        foreach (var comp in comp)
        {
            comp.ToggleOn();
        }
    }
    public void disableComps()
    {
        foreach (var comp in comp)
        {
            comp.ToggleOff();
        }
    }
}
