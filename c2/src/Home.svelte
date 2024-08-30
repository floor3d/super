<script>
  export let authenticated;
</script>

<div>
  <h1>Home</h1>
  {#if authenticated}
    <p>Welcome back, you are authenticated!</p>
  {:else}
    <p>Please log in.</p>
  {/if}
</div>

