import React, { useEffect, useState } from 'react';
import { useNavigate } from 'react-router-dom';

function UserList() {
    const [users, setUsers] = useState([]);
    const [prenom, setPrenom] = useState('');
    const [nom, setNom] = useState('');
    const [message, setMessage] = useState('');
    const navigate = useNavigate();

    useEffect(() => {
        fetch('http://localhost:8080/users')
            .then(res => res.json())
            .then(data => setUsers(data.filter(u => u !== null)))
            .catch(err => console.error('Erreur chargement utilisateurs', err));
    }, []);

    const handleCreateUser = () => {
        const nomTrim = nom.trim();
        const prenomTrim = prenom.trim();
        const nomNormalized = nomTrim.toUpperCase();
        const prenomNormalized = prenomTrim.charAt(0).toUpperCase() + prenomTrim.slice(1).toLowerCase();

        if (!nomTrim || !prenomTrim) {
            setMessage("❌ Les champs ne doivent pas être vides.");
            return;
        }

        if (nomTrim.length > 20 || prenomTrim.length > 20) {
            setMessage("❌ Maximum 20 caractères par champ.");
            return;
        }

        const userExists = users.some(u =>
            u &&
            u.nom.toUpperCase() === nomNormalized &&
            u.prenom.toLowerCase() === prenomNormalized.toLowerCase()
        );

        if (userExists) {
            setMessage("❌ Cet utilisateur existe déjà.");
            return;
        }

        fetch('http://localhost:8080/users', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({
                nom: nomNormalized,
                prenom: prenomNormalized,
                tempsTotal: 0
            })
        })
            .then(res => res.json())
            .then(newUser => {
                setUsers([...users, newUser]);
                setMessage("✅ Utilisateur créé avec succès !");
                setNom('');
                setPrenom('');
            })
            .catch(() => setMessage("❌ Erreur lors de la création."));
    };

    const handleClick = (id) => {
        navigate(`/user/${id}`);
    };

    return (
        <div>
            <h2>Veuillez choisir un utilisateur :</h2>
            <div style={{ marginBottom: '20px' }}>
                {users.map(user => (
                    <button key={user.id} onClick={() => handleClick(user.id)} style={{ margin: '5px' }}>
                        {user.nom.toUpperCase()} {user.prenom}
                    </button>
                ))}
            </div>

            <h3>Créer un nouvel utilisateur :</h3>
            <input
                type="text"
                placeholder="Prénom"
                value={prenom}
                onChange={(e) => setPrenom(e.target.value)}
                maxLength={20}
                style={{ marginRight: '5px' }}
            />
            <input
                type="text"
                placeholder="Nom"
                value={nom}
                onChange={(e) => setNom(e.target.value)}
                maxLength={20}
                style={{ marginRight: '5px' }}
            />
            <button onClick={handleCreateUser}>Créer</button>

            {message && (
                <p style={{ color: message.startsWith("✅") ? "green" : "red" }}>{message}</p>
            )}
        </div>
    );
}

export default UserList;
